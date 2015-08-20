//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media Foundation
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// Portions Copyright (c) Microsoft Open Technologies, Inc.
//
//*@@@---@@@@******************************************************************

#pragma once

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "MFIncludes.hpp"

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
    CvSize WinRTVideoCapture::getSize();


private:

    void GrabFrameAsync(::Media::CaptureFrameGrabber^ frameGrabber);
    Platform::Agile<WMC::MediaCapture> m_capture;
    std::function<void(const cv::Mat& mat)> m_callback;

    int m_width;
    int m_height;
    cv::Mat m_mat;
};

class CvCapture_WinRT : public CvCapture
{
public:
    CvCapture_WinRT() {
        m_currentframe = m_firstframe = 0;
        m_frame = 0;
    }

    virtual ~CvCapture_WinRT() {
        close();
    }

    virtual bool open();
    virtual void close();
    virtual bool grabFrame();
    virtual IplImage* retrieveFrame(int);

protected:
    unsigned m_currentframe;
    unsigned m_firstframe; // number of first frame

    IplImage* m_frame;
    HWinRTVideoCapture m_capture;
};

void CvCapture_WinRT::close() {

    m_currentframe = m_firstframe = 0;

    cvReleaseImage(&m_frame);
}


bool CvCapture_WinRT::grabFrame() {
    //cvReleaseImage(&m_frame);
    //frame = cvLoadImage(str, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
    if (m_frame)
        m_currentframe++;

    return m_frame != 0;
}

IplImage* CvCapture_WinRT::retrieveFrame(int) {
    return m_frame;
}

bool CvCapture_WinRT::open() {
    unsigned offset = 0;
    close();

    int width = 640, height = 360;
    m_capture = WinRTVideoCapture::create(width, height);

    m_frame = cvCreateImage(m_capture->getSize(), 8, 4);
    cvZero(m_frame);

    // start capturing video. Callback will happen on the UI thread
    m_capture->start([this](const cv::Mat& mat) {
        // convert to grayscale
        cv::Mat intermediateMat;
        cv::cvtColor(mat, intermediateMat, CV_RGB2GRAY);

        // convert to BGRA
        cv::Mat output;
        cv::cvtColor(intermediateMat, output, CV_GRAY2BGRA);

        if (m_frame) cvReleaseImage(&m_frame);

        // copy constructor is invoked here
        *m_frame = output;
    });

    m_firstframe = offset;
    return true;
}

CvCapture* cvCreateCameraCapture_WinRT(int index)
{
    CvCapture_WinRT* capture = new CvCapture_WinRT;

    try
    {
        if (capture->open())
            return capture;
    } catch (...)
    {
        delete capture;
        throw;
    }

    delete capture;
    return 0;
}