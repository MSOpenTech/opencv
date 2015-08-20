// TBD Copyrights stuffs.
#include "pch.h"
#include <locale>
#include <codecvt>
#include "CascadeClassifier.h"

using namespace Windows::System;

cvRT::CascadeClassifier::CascadeClassifier()
{
}

// Map to cv::CascadeClassifier.Load()
bool cvRT::CascadeClassifier::Load(String^ fileName)
{      
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string filenameNative = converter.to_bytes(fileName->Data());    
    return casCla.load(cv::String(filenameNative));    
}

// Map to cv::CascadeClassifier.detectMultiScale()
void cvRT::CascadeClassifier::detectMultiScale(Mat^ image, IVector<Rect^>^ objects, double scaleFactor, int minNeighbors, int flags, Size^ minSize)
{
    std::vector<cv::Rect> returnedObjects;

    casCla.detectMultiScale(
        image->RawMat(),
        returnedObjects,
        scaleFactor,
        minNeighbors,
        flags,
        cv::Size(minSize->Width, minSize->Height)); 

    for (size_t i = 0; i < returnedObjects.size(); i++)
    {
        cv::Rect point = returnedObjects[i];
        objects->Append(ref new Rect(point.x, point.y, point.width, point.height));
    }
}
