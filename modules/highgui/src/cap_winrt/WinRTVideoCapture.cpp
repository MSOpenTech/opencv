//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media Foundation
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// Portions Copyright (c) Microsoft Open Technologies, Inc. 
//
//*@@@---@@@@******************************************************************


#include "../../include/opencv2/highgui/cap_winrt/WinRTVideoCapture.h"
#include "CaptureFrameGrabber.h"

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Capture;
using namespace Windows::UI::Xaml::Media::Imaging;

HWinRTVideoCapture WinRTVideoCapture::create(int width, int height)
{
    return std::make_shared<WinRTVideoCapture>(width, height);
}

WinRTVideoCapture::WinRTVideoCapture(int width, int height)
    : m_width(width)
    , m_height(height)
{

}

WinRTVideoCapture::~WinRTVideoCapture()
{


}

void WinRTVideoCapture::start(const std::function<void(const cv::Mat&)>& callback)
{
    m_callback = callback;

    auto settings = ref new MediaCaptureInitializationSettings();
    settings->StreamingCaptureMode = StreamingCaptureMode::Video; // Video-only capture
    m_capture = ref new MediaCapture();
    create_task(m_capture->InitializeAsync(settings)).then([this](){

        auto props = safe_cast<VideoEncodingProperties^>(m_capture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview));
        props->Subtype = MediaEncodingSubtypes::Bgra8; // Ask for color conversion to match WriteableBitmap
        props->Width = m_width;
        props->Height = m_height;

        return ::Media::CaptureFrameGrabber::CreateAsync(m_capture.Get(), props);

    }).then([this](::Media::CaptureFrameGrabber^ frameGrabber)
    {
        GrabFrameAsync(frameGrabber);
    });

}

void WinRTVideoCapture::stop()
{


}

void WinRTVideoCapture::GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber)
{
    create_task(frameGrabber->GetFrameAsync()).then([this, frameGrabber](const ComPtr<IMF2DBuffer2>& buffer)
    {
        if (m_mat.cols != m_width || m_mat.rows != m_height)
        {
            m_mat.create(m_height, m_width, CV_8UC4);
        }

        // create a matrix the size and type of the image
        CHK(buffer->ContiguousCopyTo(m_mat.data, m_width * m_height * 4));

        // callback with the matrix containing the video frame
        m_callback(m_mat);

        // grab another frame
        GrabFrameAsync(frameGrabber);
    }, task_continuation_context::use_current());
}

