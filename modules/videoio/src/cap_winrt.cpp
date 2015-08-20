// Capture support for WinRT
// Microsoft Surface and Phone using Media Foundation

// Copyright (c) 2015, Microsoft Open Technologies, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names
//   of its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "precomp.hpp"
#include "cap_winrt.hpp"
#include "cap_winrt_highgui.hpp"

using namespace Windows::Foundation;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Devices::Enumeration;

using namespace Platform;

using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Microsoft::WRL;

using namespace ::std;


// nb. VideoCapture_WinRT is not a singleton, so the Mats are made file statics
// we do not support more than one capture device simultaneously with the
// design at this time

// nb. inputBufferMutex was not able to guarantee that OpenCV Mats were
// ready to accept data in the UI thread (memory access exceptions were thrown
// even though buffer address was good).
// Therefore allocation of Mats is also done on the UI thread before the video
// device is initialized.

static cv::Mat frontInputMat;
static cv::Mat backInputMat;

// performed on UI thread
void allocateBuffers(int width, int height)
{
    // allocate input Mats (bgra8 = CV_8UC4, RGB24 = CV_8UC3)
    frontInputMat.create(height, width, CV_8UC3);
    backInputMat.create(height, width, CV_8UC3);
    HighguiBridge::getInstance().frontInputPtr = frontInputMat.ptr(0);
    HighguiBridge::getInstance().backInputPtr = backInputMat.ptr(0);

    HighguiBridge::getInstance().AllocateOutputBuffers();
}


namespace cv {

    VideoCapture_WinRT::VideoCapture_WinRT(int device) : started(false)
    {
        HighguiBridge::getInstance().deviceIndex = device;
    }

    bool VideoCapture_WinRT::isOpened() const
    {
        return true; // started;
    }

    // grab a frame:
    // this will NOT block per spec
    // should be called on the image processing thread, not the UI thread
    bool VideoCapture_WinRT::grabFrame()
    {
        // if device is not started we must return true so retrieveFrame() is called to start device
        // nb. we cannot start the device here because we do not know the size of the input Mat
        if (!started) return true;

        if (HighguiBridge::getInstance().bIsFrameNew)
        {
            return true;
        }

        // nb. if blocking is to be added:
        // unique_lock<mutex> lock(HighguiBridge::getInstance().frameReadyMutex);
        // HighguiBridge::getInstance().frameReadyEvent.wait(lock);
        return false;
    }

    // should be called on the image processing thread after grabFrame
    // see VideoCapture::read
    bool VideoCapture_WinRT::retrieveFrame(int channel, cv::OutputArray outArray)
    {
        if (!started) {

            int width, height;
            width = outArray.size().width;
            height = outArray.size().height;         

            if (width == 0) width = 640;
            if (height == 0) height = 480;

            HighguiBridge::getInstance().width = width;
            HighguiBridge::getInstance().height = height;

            // nb. Mats will be alloc'd on UI thread
			
            // [stjong] 
            // tbd, the reporter may get ahead of reporting status before MainPage() adds handler to process
            // the action (see samples\winrt_universal\highgui_xaml\highgui_xaml\highgui_xaml.Windows\MainPage.xaml.cpp asyncTask->Progress(). 
            // Yield here to avoid the race.
            Sleep(3000);

            // request device init on UI thread - this does not block, and is async
            HighguiBridge::getInstance().requestForUIthreadAsync(OPEN_CAMERA,
                outArray.size().width, outArray.size().height);

            started = true;
            return false;
        }

        if (!started) return false;

        return HighguiBridge::getInstance().bIsFrameNew;
    }


    bool VideoCapture_WinRT::setProperty(int property_id, double value)
    {
        switch (property_id)
        {
        case CAP_PROP_FRAME_WIDTH:
            size.width = (int)value;
            break;
        case CAP_PROP_FRAME_HEIGHT:
            size.height = (int)value;
            break;
        default:
            return false;
        }
        return true;
    }
}


// end