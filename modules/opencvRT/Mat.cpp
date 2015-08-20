// TBD copyrights stuffs

#include "pch.h"

using namespace cvRT;

Mat::Mat()
{
	_rawMatHolder = new cv::Mat();
}

Mat::Mat(cv::Mat& rawMat)
{
	_rawMatHolder = new cv::Mat(rawMat);    
}

Mat^ Mat::RectSubMettric(cvRT::Rect^ rect)
{
    return ref new Mat((*_rawMatHolder)(rect->GetCvRect()));
}

unsigned int Mat::total()
{
    return _rawMatHolder->total();
}
