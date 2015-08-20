#pragma once
// TBD Copyrights stuffs.

#include <opencv2/videoio.hpp>

using namespace cv;

namespace VideoIoCx
{
    public ref class MatCx sealed
    {
    public:
        MatCx();
    
    internal:
        cv::Mat* GetMat() { return &cvMat; }

    private:
        cv::Mat cvMat;        
    };
}

