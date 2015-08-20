// TBD Copyrights stuffs.

#pragma once

namespace cvRT
{
    public enum class CV_HAAR
    {
        DO_CANNY_PRUNING = CV_HAAR_DO_CANNY_PRUNING,
        SCALE_IMAGE = CV_HAAR_SCALE_IMAGE,
        FIND_BIGGEST_OBJECT = CV_HAAR_FIND_BIGGEST_OBJECT,
        DO_ROUGH_SEARCH = CV_HAAR_DO_ROUGH_SEARCH
    };    

    // tbd better naming
    public enum class CASCADE
    {      
        CASCADE_DO_CANNY_PRUNING = 1,
        CASCADE_SCALE_IMAGE = 2,
        CASCADE_FIND_BIGGEST_OBJECT = 4,
        CASCADE_DO_ROUGH_SEARCH = 8
    };    

    public ref class CascadeClassifier sealed
    {
    public:
        CascadeClassifier();
        
        bool Load(String^ fileName);
           
        void detectMultiScale(
            Mat^ image,
            IVector<Rect^>^ objects,
            double scaleFactor,
            int minNeighbors, 
            int flags,
            Size^ minSize);      

    protected:


    private:
        cv::CascadeClassifier casCla;


    };
}
