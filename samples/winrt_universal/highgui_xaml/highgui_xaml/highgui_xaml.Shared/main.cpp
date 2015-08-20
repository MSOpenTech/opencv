// main.cpp

// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names 
//   of its contributors may be used to endorse or promote products derived 
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>


#include <vector>

#include "../src/cap_winrt_highgui.hpp"

using namespace cv;
using namespace std;


// 0 = passthrough
// 1 = gray
// 2 = shades
// 3 = canny
// 4 = contour
// 5 = object detect
int ProcessingMethodIndex = 5;

void cvMain()
{
    VideoCapture cam;

    OutputDebugString(L"hello world 1 \r\n");

    cam.open(0);    // open the default camera

    Mat edges;
    //namedWindow("edges", 1);

    //int sliderValue = 0;
    //createTrackbar("", "", &sliderValue, 100);

    Mat frame;
    int row = 0;
    int column = 0;


    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;

    // process frames
    while (1)
    {
        // get a new frame from camera - this is non-blocking per spec
        cam >> frame;

        // don't reprocess the same frame again
        // nb if commented then flashing may occur
        if (!cam.grab()) continue;

        if (frame.total() == 0)
            continue;

        // image processing calculations here
        // nb Mat frame is in RGB24 format (8UC3)

        OutputDebugStringA("processing frame...\r\n");

        switch (ProcessingMethodIndex)
        {
        case 0:
            // just passthrough..
            break;

            // gray
        case 1:
            cvtColor(frame, edges, COLOR_RGB2GRAY);
            cvtColor(edges, frame, COLOR_GRAY2RGB);
            break;

            // shades
        case 2:
        {
            // image manipulation test 1
            // write color bar at row 100 for 200 rows
            // slider adjusts color
            row += 10;

            if (row + 100 > 480)
            {
                row = 0;
            }

            auto ar = frame.ptr(row);

            int bytesPerPixel = 3;

            // int adjust = (int)(((float)sliderValue / 100.0f) * 255.0);

            int adjust = (int)(((float)30 / 100.0f) * 255.0);

            for (int i = 0; i < 640 * 100 * bytesPerPixel;)
            {
                ar[i++] = adjust;           // R
                i++;                        // G
                ar[i++] = 255 - adjust;     // B
            }



            for (int row = 0; row < 480; row++)
            {
                ar = frame.ptr(row, column);
                int columnToDraw = min(640 - column, 100);
                for (int i = 0; i < columnToDraw * bytesPerPixel;)
                {
                    ar[i++] = adjust;           // R
                    i++;                        // G
                    ar[i++] = 255 - adjust;     // B
                }
            }

            column += 20;
            if (column >= 640)
                column = 0;
            break;
        }

        // canny
        case 3:
        {
            // image processing test 2
            // slider adjusts upper threshold		
            cvtColor(frame, edges, COLOR_RGB2GRAY);
            GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);

            Canny(edges, edges, 0, 30, 3);

            //Canny(edges, edges, 0, sliderValue, 3);
            cvtColor(edges, frame, COLOR_GRAY2RGB);

            break;
        }

        // contour
        case 4:
        {
            Mat canny_output;
            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;
            int thresh = 100;
            RNG rng(12345);

            Canny(frame, canny_output, thresh, thresh * 2, 3);
            findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

            // Mat drawing = Mat::zeros(frame.size(), CV_8UC3);

            frame = Scalar(0, 0, 0);

            for (size_t i = 0; i < contours.size(); i++)
            {
                Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
                drawContours(frame, contours, (int)i, color, 2, 8, hierarchy, 0, Point());
            }

            break;
        }

        case 5:
        {
            static bool firstTime = true;
            String face_cascade_name = "haarcascade_frontalface_alt.xml";
            String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
            if (firstTime == true)
            {
                if (!face_cascade.load(face_cascade_name)) { OutputDebugStringA("bleh"); }
                if (!eyes_cascade.load(eyes_cascade_name)) { OutputDebugStringA("blah"); }
                firstTime = false;
            }

            std::vector<Rect> faces;
            Mat frame_gray;

            cvtColor(frame, frame_gray, CV_BGR2GRAY);
            equalizeHist(frame_gray, frame_gray);

            //-- Detect faces
            try
            {
                face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
            }
            catch (exception e)
            {
                char output[1024] = { 0 };
                (void)sprintf_s(output, 1024, "error: [%s]\n", e.what());
                OutputDebugStringA(output);
            }

            for (size_t i = 0; i < faces.size(); i++)
            {
                Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
                ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
                Mat faceROI = frame_gray(faces[i]);

                std::vector<Rect> eyes;
                //-- In each face, detect eyes
                eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

                for (size_t j = 0; j < eyes.size(); j++)
                {
                    Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
                    int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
                    circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);
                }
            }
        }
        }

        //imshow("", frame);
        imshow_winrt(frame);
    }
}

