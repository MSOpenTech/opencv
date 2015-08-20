// Highgui for OpenCV to XAML
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
#include "cap_winrt.hpp"
#include "cap_winrt_highgui.hpp"
#include "cap_winrt_video.hpp"

using namespace Windows::Foundation;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Devices::Enumeration;

using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Microsoft::WRL;

using namespace Platform;
using namespace ::Concurrency;

using namespace ::std;

void imshow_winrt(cv::InputArray matToShow)
{
    HighguiBridge::getInstance().imshow(matToShow);
}

// non-blocking
bool initGrabber(int device, int w, int h)
{
    // nb. Video class is not exported outside of this DLL
    // due to complexities in the CaptureFrameGrabber ref class
    // as written in the header not mixing well with pure C++ classes
    return Video::getInstance().initGrabber(device, w, h);
}

void closeGrabber()
{
    Video::getInstance().closeGrabber();
}

// non-blocking
void HighguiBridge::requestForUIthreadAsync(int action, int widthp, int heightp)
{
    reporter.report(action);
}

HighguiBridge& HighguiBridge::getInstance()
{
    static HighguiBridge instance;
    return instance;
}

void HighguiBridge::SwapInputBuffers()
{
    // already locked
    // lock_guard<mutex> lock(inputBufferMutex);
    swap(backInputPtr, frontInputPtr);
    //if (currentFrame != frameCounter)
    //{
    //    currentFrame = frameCounter;
    //    swap(backInputPtr, frontInputPtr);
    //}
}

void HighguiBridge::SwapOutputBuffers()
{
    lock_guard<mutex> lock(outputBufferMutex);
    swap(frontOutputBuffer, backOutputBuffer);
}

void HighguiBridge::createTrackbar( int *valptr )
{
    slider1ValPtr = valptr;
    HighguiBridge::getInstance().requestForUIthreadAsync(SHOW_TRACKBAR);
}

void HighguiBridge::AllocateOutputBuffers()
{
    frontOutputBuffer = ref new WriteableBitmap(width, height);
    backOutputBuffer = ref new WriteableBitmap(width, height);
}

void HighguiBridge::imshow(cv::InputArray img)
{
    HighguiBridge::getInstance().SwapOutputBuffers();
    HighguiBridge::getInstance().requestForUIthreadAsync(UPDATE_IMAGE_ELEMENT);
}

// nb on UI thread
void copyOutput()
{
    Video::getInstance().CopyOutput();
}

// nb on UI thread
//void sliderChanged1(double value)
//{
//    auto i = (int)value;
//    if (HighguiBridge::getInstance().slider1ValPtr != nullptr)
//        *HighguiBridge::getInstance().slider1ValPtr = i;
//}

// end