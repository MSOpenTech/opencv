// TBD copyrights stuffs

#pragma once

namespace cvRT
{
    public ref class Mat sealed
    {
    public:
        Mat();
        unsigned int total();

        // tbd need better way
        Mat^ RectSubMettric(cvRT::Rect^ rect);

    internal:
        Mat(cv::Mat& rawMat);
        Mat(cv::Mat* rawMat);
        cv::Mat& RawMat()
        {
            return *_rawMatHolder;
        }

    private:
        cv::Mat* _rawMatHolder;
    };
}

