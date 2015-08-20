// TBD Copyrights stuffs.

#include "pch.h"
#include <ppl.h>
#include <ppltasks.h>
#include <concrt.h>
#include <agile.h>
#include <opencv2/videoio.hpp>
#include "../src/cap_winrt_highgui.hpp"
#include "MatCx.h"
#include "VideoIoCx.h"

//#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/features2d.hpp>
//#include <opencv2/objdetect.hpp>
//#include <vector>

// nb. path relative to modules/videoio/include


using namespace Platform;
using namespace ::concurrency;
using namespace ::Windows::Foundation;
using namespace ::Windows::UI::Xaml::Controls;
using namespace cv;
using namespace VideoIoCx;

VideoIo::VideoIo()
{    
}


void VideoIo::Initialize()
{
    auto asyncTask = create_async([this](progress_reporter<int> reporter)
                    {
                        HighguiBridge::getInstance().setReporter(reporter);
                        // cvMain();
                    });

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

void VideoIo::SetImage(Windows::UI::Xaml::Controls::Image^ cvImage)
{
    HighguiBridge::getInstance().cvImage = cvImage;
}

void VideoIo::StartCapture()
{
    vidCap.open(0);
}


void VideoIo::StopCapture()
{
    // tbd

}

/*
IAsyncActionWithProgress<int>^ VideoIo::TaskWithProgressAsync()
{
    return 
}
*/


void VideoIo::GetFrame(MatCx^ frame)
{    
    // [stjong] tbd can't loop forever here.
    while (1)
    {
        vidCap >> *frame->GetMat();

        // tbd these are from original sample.  need to look more into this.
        //if (!vidCap.grab()) 
        //   continue;

        // ditto as above.
        //if (frame->GetMat()->total() == 0)
        //    continue;

        break;
    }

    OutputDebugString(L"Frame obtained");
}

void VideoIo::ShowFrame(MatCx^ frame)
{
    imshow_winrt(*frame->GetMat());
}




void VideoIo::cvMain()
{
    VideoCapture cam;

    OutputDebugString(L"hello world 1 \r\n");

    cam.open(0);    // open the default camera   
        
    MatCx frame;

    int row = 0;
    int column = 0;
    
    int ProcessingMethodIndex = 0;

    // process frames
    while (1)
    {
        cv::Mat* mat = frame.GetMat();
        
        // get a new frame from camera - this is non-blocking per spec
        cam >> *frame.GetMat();

        // don't reprocess the same frame again
        // nb if commented then flashing may occur
        if (!cam.grab()) continue;

        if (frame.GetMat()->total() == 0)
            continue;

        // image processing calculations here
        // nb Mat frame is in RGB24 format (8UC3)

        OutputDebugStringA("processing frame...\r\n");

        switch (ProcessingMethodIndex)
        {
        case 0:
            // just passthrough..
            break;

        default:
            break;
        }

        //imshow("", frame);
        imshow_winrt(*frame.GetMat());
    }


    OutputDebugString(L"Hello Main Loop");
}

    


