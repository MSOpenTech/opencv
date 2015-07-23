﻿// TBD Copyrights stuffs.

#include "pch.h"
#include <ppl.h>
#include <ppltasks.h>
#include <concrt.h>
#include <agile.h>
#include <opencv2/videoio.hpp>
// nb. path relative to modules/videoio/include
#include "../src/cap_winrt_highgui.hpp"
#include "MatCx.h"
#include "VideoIoCx.h"

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
                break;
            }           

            case CLOSE_CAMERA:
            {
                closeGrabber();
                break;
            }

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
    vidCap.release();
}

void VideoIo::GetFrame(MatCx^ frame)
{    
    // [stjong] tbd can't loop forever here.
    while (1)
    {
        vidCap >> *frame->GetMat();

        // tbd these are from original sample.  need to look more into this.
        if (!vidCap.grab()) 
           continue;

        // ditto as above.
        if (frame->GetMat()->total() == 0)
            continue;

        break;
    }

    OutputDebugString(L"Frame obtained");
}

void VideoIo::ShowFrame(MatCx^ frame)
{
    imshow_winrt(*frame->GetMat());
}

