// TBD Copyrights stuffs.
#include "pch.h"

#include "../src/cap_winrt_highgui.hpp"
#include "VideoIo.h"

// nb. path relative to modules/videoio/include

using namespace ::concurrency;
using namespace ::Windows::Foundation;
using namespace ::Windows::UI::Xaml::Controls;
// using namespace cv;
using namespace cvRT;

VideoIo::VideoIo()
{
}

void VideoIo::Initialize()
{
	auto asyncTask = create_async([this](progress_reporter<int> reporter)
						{
							HighguiBridge::getInstance().setReporter(reporter);
						});

	asyncTask->Progress = ref new AsyncActionProgressHandler<int>([this](IAsyncActionWithProgress<int>^ act, int progress)
	{
		int action = progress;

		// these actions will be processed on the UI thread asynchronously
		switch (action)
		{
		case OPEN_CAMERA:
		{
			int device = HighguiBridge::getInstance().deviceIndex;
			int width = HighguiBridge::getInstance().width;
			int height = HighguiBridge::getInstance().height;

			// buffers must alloc'd on UI thread
			allocateBuffers(width, height);

			// nb. video capture device init must be done on UI thread;
			// code is located in the OpenCV Highgui DLL, class Video
			if (!grabberStarted)
			{
				grabberStarted = true;
				initGrabber(device, width, height);
			}
		}
		break;
		case CLOSE_CAMERA:
			closeGrabber();
			break;
		case UPDATE_IMAGE_ELEMENT:
		{
			// copy output Mat to WBM
			copyOutput();

			// set XAML image element with image WBM
			HighguiBridge::getInstance().cvImage->Source = HighguiBridge::getInstance().backOutputBuffer;
		}
		break;

		//case SHOW_TRACKBAR:
		//    cvSlider->Visibility = Windows::UI::Xaml::Visibility::Visible;
		//    break;

		}
	});
}

void VideoIo::SetImage(Windows::UI::Xaml::Controls::Image^ cvImage)
{
	HighguiBridge::getInstance().cvImage = cvImage;
}

void VideoIo::StartCapture()
{
	vidCap.open(0);
}


void VideoIo::StopCapture()
{
	vidCap.release();
}

void VideoIo::GetFrame(Mat^ frame)
{
	// [stjong] tbd can't loop forever here.
	while (1)
	{
		vidCap >> frame->RawMat();

		// tbd these are from original sample.  need to look more into this.
		if (!vidCap.grab())
			continue;

		// ditto as above.
		if (frame->RawMat().total() == 0)
			continue;

		break;
	}

	OutputDebugString(L"Frame obtained");
}

void VideoIo::ShowFrame(Mat^ frame)
{
	imshow_winrt(frame->RawMat());
}