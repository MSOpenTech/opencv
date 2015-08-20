#pragma once

#include <opencv2\videoio.hpp>

// Renders Direct2D and 3D content on the screen.
namespace VideoCapture
{
    class AppMain
    {
    public:
        AppMain(Windows::UI::Xaml::Controls::Image^ image);
        ~AppMain();
        void start();
        void start(int width, int height);
        void stop();
        void showFrame();
        Concurrency::task<cv::Mat> getFrameAsync();

    private:
        Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ m_bitmap;
        unsigned int m_width;
        unsigned int m_height;
        std::function<void(const cv::Mat& mat)> m_callback;
        cv::VideoCapture m_cvCapture;

        Windows::UI::Xaml::Controls::Image^ m_image;
        Windows::UI::Core::CoreDispatcher^ m_dispatcher;
        cv::Mat m_frame;
    };
}