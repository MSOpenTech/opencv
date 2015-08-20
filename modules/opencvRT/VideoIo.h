// TBD Copyrights stuffs.

#pragma once

namespace cvRT
{
	public ref class VideoIo sealed
	{
	public:
		VideoIo();
		void SetImage(Windows::UI::Xaml::Controls::Image^ cvImage);
		void Initialize();
		void StartCapture();
		void StopCapture();
		void GetFrame(Mat^ frame);
		void ShowFrame(Mat^ frame);

	protected:
		// Windows::Foundation::IAsyncActionWithProgress<int>^ TaskWithProgressAsync();

	private:
		bool grabberStarted;    // tbd rename this to better variable
		cv::VideoCapture vidCap;    // tbd comment
		Windows::Foundation::IAsyncActionWithProgress<int>^ asyncTask;   // tbd comment
	};
}