#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#ifndef MY_FACEMEMORY // include guard
#define MY_FACEMEMORY
class faceMemory
{
public:
	cv::Rect rect;
	cv::Mat image;
	int counter = 0;
	bool isRecorded = false;
	bool isToSwap = false;
	std::vector<cv::Mat> recordedImages;

	cv::Point getCenter();

	faceMemory(cv::Rect _rect, cv::Mat image);
	~faceMemory();
};

#endif