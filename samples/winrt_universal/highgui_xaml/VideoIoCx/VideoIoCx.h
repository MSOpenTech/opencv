// TBD Copyrights stuffs.

#pragma once

// #include "MatCx.h"

namespace VideoIoCx
{
    public ref class VideoIo sealed
    {
    public:
        VideoIo();
        void SetImage(Windows::UI::Xaml::Controls::Image^ cvImage);
        void Initialize();
        void StartCapture();
        void StopCapture();
        void GetFrame(MatCx^ frame);
        void ShowFrame(MatCx^ frame);
        
    protected:
        // Windows::Foundation::IAsyncActionWithProgress<int>^ TaskWithProgressAsync();

    private:
        void VideoIo::cvMain(); // tbd push this up to C#
        bool grabberStarted;    // tbd rename this to better variable
        VideoCapture vidCap;    // tbd comment
        Windows::Foundation::IAsyncActionWithProgress<int>^ asyncTask;   // tbd comment
    };
}

