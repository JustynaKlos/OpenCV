#include "faceMemory.h"

faceMemory::faceMemory(cv::Rect _rect, cv::Mat _image)
{
	image = _image;
	rect = _rect;
}


cv::Point faceMemory::getCenter() {
	return cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
}



faceMemory::~faceMemory()
{
}
