//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

// nb. path relative to modules/videoio/include
#include "../src/cap_winrt_highgui.hpp"

using namespace highgui_xaml;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

#include <ppl.h>
#include <ppltasks.h>
#include <concrt.h>
#include <agile.h>

using namespace ::concurrency;
using namespace ::Windows::Foundation;

using namespace Windows::UI::Xaml::Media::Imaging;


namespace highgui_xaml
{


    MainPage::MainPage()
    {
        InitializeComponent();

        grabberStarted = false;

        Window::Current->VisibilityChanged += ref new Windows::UI::Xaml::WindowVisibilityChangedEventHandler(this, &highgui_xaml::MainPage::OnVisibilityChanged);

        // set XAML elements
        HighguiBridge::getInstance().cvImage = cvImage;
        //HighguiBridge::getInstance().cvSlider = cvSlider;

        // handler
        //cvSlider->ValueChanged +=
        //    ref new RangeBaseValueChangedEventHandler(this, &MainPage::cvSlider_ValueChanged);

        auto asyncTask = TaskWithProgressAsync();
        asyncTask->Progress = ref new AsyncActionProgressHandler<int>([this](IAsyncActionWithProgress<int>^ act, int progress)
        {
            int action = progress;

            // these actions will be processed on the UI thread asynchronously
            switch (action)
            {
            case OPEN_CAMERA:
                {
                    int device = HighguiBridge::getInstance().deviceIndex;
                    int width = HighguiBridge::getInstance().width;
                    int height = HighguiBridge::getInstance().height;

                    // buffers must alloc'd on UI thread
                    allocateBuffers(width, height);

                    // nb. video capture device init must be done on UI thread;
                    // code is located in the OpenCV Highgui DLL, class Video
                    if (!grabberStarted)
                    {
                        grabberStarted = true;
                        initGrabber(device, width, height);
                    }
                }
                break;
            case CLOSE_CAMERA:
                closeGrabber();
                break;
            case UPDATE_IMAGE_ELEMENT:
                {
                    // copy output Mat to WBM
                    copyOutput();

                    // set XAML image element with image WBM
                    HighguiBridge::getInstance().cvImage->Source = HighguiBridge::getInstance().backOutputBuffer;
                }
                break;
            //case SHOW_TRACKBAR:
            //    cvSlider->Visibility = Windows::UI::Xaml::Visibility::Visible;
            //    break;
            }
        });
    }

    //void MainPage::cvSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
    //{
    //    sliderChanged1(e->NewValue);
    //}
}

// nb. implemented in main.cpp
void cvMain();

// set the reporter method for the HighguiAssist singleton
// start the main OpenCV as an async thread in WinRT
IAsyncActionWithProgress<int>^ MainPage::TaskWithProgressAsync()
{
    return create_async([this](progress_reporter<int> reporter)
    {
        HighguiBridge::getInstance().setReporter(reporter);
        cvMain();
    });
}

void highgui_xaml::MainPage::OnVisibilityChanged(Platform::Object ^sender,
    Windows::UI::Core::VisibilityChangedEventArgs ^e)
{
    if (e->Visible)
    {
        // only start the grabber if the camera was opened in OpenCV
        if (HighguiBridge::getInstance().backInputPtr != nullptr)
        {
            if (grabberStarted) return;

            int device = HighguiBridge::getInstance().deviceIndex;
            int width = HighguiBridge::getInstance().width;
            int height = HighguiBridge::getInstance().height;

            initGrabber(device, width, height);
        }
    }
    else
    {
        grabberStarted = false;
        closeGrabber();
    }
}


void highgui_xaml::MainPage::comboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	extern int ProcessingMethodIndex;

	char output[1024] = { 0 };
	(void)sprintf_s(output, 1024, "New selection [%d]\n", MyComboBox->SelectedIndex);
	OutputDebugStringA(output);

	ProcessingMethodIndex = MyComboBox->SelectedIndex;	
}
