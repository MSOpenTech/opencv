//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media Foundation
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// Portions Copyright (c) Microsoft Open Technologies, Inc. 
//
//*@@@---@@@@******************************************************************

#pragma once

#include <opencv2\core\core.hpp>

class WinRTVideoCapture;

typedef std::shared_ptr<WinRTVideoCapture> HWinRTVideoCapture;

namespace Media
{
    ref class CaptureFrameGrabber;
}


class WinRTVideoCapture
{
public:
    static HWinRTVideoCapture create(int width, int height);
    WinRTVideoCapture(int width, int height);
    ~WinRTVideoCapture();
    void start(const std::function<void(const cv::Mat& mat)>& callback);
    void stop();


private:

    void GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber);
    Platform::Agile<WMC::MediaCapture> m_capture;
    std::function<void(const cv::Mat& mat)> m_callback;

    int m_width;
    int m_height;
    cv::Mat m_mat;
};
