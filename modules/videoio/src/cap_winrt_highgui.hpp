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

#pragma once

// this header is included in the XAML App, so it cannot include any
// OpenCV headers, or a static assert will be raised

#include <ppl.h>
#include <ppltasks.h>
#include <concrt.h>
#include <agile.h>
#include <opencv2\core.hpp>

#include <mutex>
#include <memory>
#include <atomic>
#include <functional>
 

enum {
    OPEN_CAMERA = 300,
    CLOSE_CAMERA,
    UPDATE_IMAGE_ELEMENT,
    SHOW_TRACKBAR
};


// class HighguiBridge is needed because the interface for
// VideoCapture_WinRT in cap_winrt.hpp is fixed by OpenCV

__declspec(dllexport) bool initGrabber(int device, int w, int h);
__declspec(dllexport) void closeGrabber();
__declspec(dllexport) void copyOutput();
//__declspec(dllexport) void sliderChanged1(double value);
__declspec(dllexport) void allocateBuffers(int width, int height);

__declspec(dllexport) void imshow_winrt(cv::InputArray matToShow);


// singleton
class HighguiBridge
{
public:

// common methods for all DLLs:

    __declspec(dllexport) static HighguiBridge& getInstance();

    // call after initialization
    void setReporter(Concurrency::progress_reporter<int> pr) { reporter = pr; }

    // to be called from cvMain via cap_winrt on bg thread - non-blocking (async)
    void requestForUIthreadAsync( int action, int width=0, int height=0 );

// highgui UI interface:

    void createTrackbar( /*void *callbackOnChange() */);   // unhides trackbar and registers OpenCV callback
    void createTrackbar( int* valptr );             // unhides trackbar and sets value
    void setTrackbarPos(int pos) {}                 // unhides trackbar and sets its position

    void imshow(cv::InputArray matToShow);          // shows Mat in the cvImage element
    // TODO: modify in window.cpp: void cv::imshow( const String& winname, InputArray _img )

    // namedWindow                                  // no op: only one XAML window used
    // destroyWindow                                // no op
    // destroyAllWindows                            // no op
    void MoveWindow( /* int x, int y */) {}         // moves the cvImage element on MainPage
    void ResizeWindow( /* int w, int h */) {}       // resizes the cvImage element on MainPage

    void setMouseCallback( /* callback, userdata */) {}
    // calls OpenCV callback when mouse moves inside of cvImage element on MainPage

    int waitKey(int delay = 0) { return 0; }

    // setOpenGlDrawCallback                        // no op
    // setOpenGlContext                             // no op
    // updateWindow                                 // no op

// end highgui UI interface


// private:
    // TODO: write getters/setters or specify friend classes

    int                         deviceIndex, width, height;

    std::atomic<bool>           bIsFrameNew;

    // input is double buffered
    std::mutex                  inputBufferMutex;
    unsigned char *             frontInputPtr;                              // OpenCV reads this
    unsigned char *             backInputPtr;                               // video grabber writes this
    void                        SwapInputBuffers();

    // output is double buffered
    std::mutex                  outputBufferMutex;
    Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ frontOutputBuffer;  // OpenCV write this
    Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ backOutputBuffer;   // XAML reads this
    void                        AllocateOutputBuffers();
    __declspec(dllexport) void  SwapOutputBuffers();

    std::atomic<unsigned long>  frameCounter;
    unsigned long               currentFrame;

    Windows::UI::Xaml::Controls::Image ^cvImage;
    Windows::UI::Xaml::Controls::Slider ^cvSlider;

    int *                       slider1ValPtr;

private:

    HighguiBridge() {
        deviceIndex = 0;
        width = 640;
        height = 480;
        UIthreadTaskDone = false;
        deviceReady = false;
        bIsFrameNew = false;
        currentFrame = 0;
        frameCounter = 0;
        slider1ValPtr = nullptr;
    };

    std::atomic<bool>           deviceReady;

    // singleton
    HighguiBridge(HighguiBridge const &);
    void operator=(const HighguiBridge &);

    Concurrency::progress_reporter<int> reporter;

    std::atomic<bool> UIthreadTaskDone;
};