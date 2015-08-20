#include "pch.h"
#include "AppMain.h"
#include "MainPage.g.h"

using namespace VideoCapture;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Microsoft::WRL;
using namespace Windows::Media::MediaProperties;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage::Streams;

using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::Storage;
using namespace Windows::Devices::Enumeration;
using namespace concurrency;


#include <opencv2\imgproc\types_c.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\videoio.hpp>
#include <memory>

static const int sWidth = 640;
static const int sHeight = 360;

AppMain::AppMain(Image^ image)
    : m_image(image)
    , m_width(sWidth)
    , m_height(sHeight)
{
    m_frame.create(m_height, m_width, CV_8UC4);
    image->Width = sWidth;
    image->Height = sHeight;
    m_dispatcher = Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher;
}

AppMain::~AppMain()
{

}

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw Exception::CreateException(hr);
    }
}

// Helper function to get pointer to WriteableBitmap pixel buffer
byte* GetPointerToPixelData(IBuffer^ buffer)
{
    // Cast to Object^, then to its underlying IInspectable interface.
    Object^ obj = buffer;
    ComPtr<IInspectable> insp(reinterpret_cast<IInspectable*>(obj));

    // Query the IBufferByteAccess interface.
    ComPtr<IBufferByteAccess> bufferByteAccess;
    ThrowIfFailed(insp.As(&bufferByteAccess));

    // Retrieve the buffer data.
    byte* pixels = nullptr;
    ThrowIfFailed(bufferByteAccess->Buffer(&pixels));
    return pixels;
}

void AppMain::start(int width, int height)
{
    m_width = width;
    m_height = height;
    start();
}

void AppMain::showFrame()
{
    create_task(getFrameAsync()).then([this](cv::Mat frame)
    {
        if (!m_frame.empty()) {
            // Executing code on UI thread
            // NOTE: CoreDispatcher::RunAsync() fails compilation
            m_dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                //ref new DispatchedHandler([this, frame]()
                ref new DispatchedHandler([=]()
                    {
                        m_callback(m_frame);
                    }, CallbackContext::Any
                )
             );
        }

        showFrame();
    }, task_continuation_context::use_arbitrary());
}

Concurrency::task<cv::Mat> AppMain::getFrameAsync()
{
    return create_task([this]() -> cv::Mat
    {
        //cv::Mat frame;
        m_cvCapture >> m_frame; // get a new frame from camera

        return m_frame;
    }, task_continuation_context::use_arbitrary());

}

void AppMain::start()
{
    // create a WriteableBitmap
    m_bitmap = ref new WriteableBitmap(m_width, m_height);

    // create the Video Capture device
    m_cvCapture = cv::VideoCapture(0); // open the default camera
    auto hcap = std::make_shared<cv::VideoCapture>(m_cvCapture);

    m_callback = [this](const cv::Mat& mat)
    {
        //cv::Mat mat = rt_mat->GetMat;
        if (mat.empty()) return;

        auto buffer = m_bitmap->PixelBuffer;
        auto pointer = GetPointerToPixelData(buffer);
        memcpy(pointer, mat.data, m_width * m_height * mat.step.buf[1]);

        // display the image
        m_image->Source = m_bitmap;
        m_bitmap->Invalidate();
    };

    showFrame();
}

void AppMain::stop()
{

}