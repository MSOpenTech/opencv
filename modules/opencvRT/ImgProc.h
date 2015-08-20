#pragma once

namespace cvRT
{
    public enum class ColorConversionCodes
    {
        COLOR_RGBA2GRAY = cv::COLOR_RGBA2GRAY,
        COLOR_GRAY2RGB  = cv::COLOR_GRAY2RGB
    };

    public enum class ContourRetrievalAlgorithm
    {
        RETR_EXTERNAL = cv::RETR_EXTERNAL,
        RETR_LIST = cv::RETR_LIST,
        RETR_CCOMP = cv::RETR_CCOMP,
        RETR_TREE = cv::RETR_TREE,
        RETR_FLOODFILL = cv::RETR_FLOODFILL
    };

    public enum class ContourApproximationModes
    {
        CHAIN_APPROX_NONE = cv::CHAIN_APPROX_NONE,
        CHAIN_APPROX_SIMPLE = cv::CHAIN_APPROX_SIMPLE,
        CHAIN_APPROX_TC89_L1 = cv::CHAIN_APPROX_TC89_L1,
        CHAIN_APPROX_TC89_KCOS = cv::CHAIN_APPROX_TC89_KCOS
    };

    public ref class ImgProc sealed
    {
    public:
        static void cvtColor(Mat^ src, Mat^ dst, ColorConversionCodes conversionCode);        
        static void GaussianBlur(Mat^ src, Mat^ dst, Size^ ksize, double sigmaX);
        static void GaussianBlur(Mat^ src, Mat^ dst, Size^ ksize, double sigmaX, double sigmaY);
        
        static void Canny(Mat^ src, Mat^ dst, double threshold1, double threshold2);
        static void Canny(Mat^ src, Mat^ dst, double threshold1, double threshold2, int apertureSize);
        static void Canny(Mat^ src, Mat^ dst, double threshold1, double threshold2, int apertureSize, bool L2gradient);
        static void EqualizeHist(Mat^ src, Mat^ dst);
        static void Ellipse(Mat^ src, Point^ center, Size^ axes, double angle, double start_angle, double end_angle, Scalar^ scalar, int thickness, int line_type, int shift);
        static void Circle(Mat^ src, Point^ center, int radius, Scalar^ scalar, int thickness, int line_type, int shift);     


		// Contour		
        static void FindContours(Mat^ image, IVector<IVector<Point^>^>^ contours, IVector<Vec4i^>^ hierarchy, ContourRetrievalAlgorithm mode, ContourApproximationModes method, Point^ offset);
        static void DrawContours(Mat^ image, IVector<IVector<Point^>^>^ contours, int contourIdx, Scalar^ color, int thickness, int lineType, IVector<Vec4i^>^ hierarchy, int maxLevel, Point^ offset);
               

        /*

        IVector<IVector<Point ^>>

        static void findContours(Mat^ image, IVector<IVector<Point^>^>^ contours, int mode, int method, Point offset);

		
		static void drawContours(Mat^ image, IVector<IVector<Point>^>^ contours, int contourIdx, Windows::UI::Color color, int thickness, int lineType,        
            Mat^ hierarchy, int maxLevel, Point offset);
        */

    };
}
