#include <opencv2/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <stdio.h>
#include "faceMemory.h";

#pragma warning(disable:4996)
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <string.h>
#include <shobjidl.h>
#include <tchar.h>

using namespace std;
using namespace cv;

String path = "C:/opencv/sources/data/haarcascades/";
String
face_cascade_nazwa = String(path + "haarcascade_frontalface_alt.xml"),
eyes_cascade_nazwa = String(path + "haarcascade_eye_tree_eyeglasses.xml");
CascadeClassifier face_cascade, eyes_cascade;
int klawisz;
int number = 2;
Mat face0;
VideoCapture cap;
int scaleFactor = 15, scaleFactor_min = 15, scaleFactor_max = 100,
minNeighbors = 3, minNeighbors_min = 3, minNeighbors_max = 10,
minSize = 100, minSize_min = 100, minSize_max = 450,
frameAmount = 30, frameAmount_min = 20, frameAmount_max = 200;
bool openCamera = false, openFilm = false, circleOption = false, blackEyesOption = false, bluredFaceOption = false, faceSwapOption = false,
recordOption = false, finishRecord = false, turnOn = false, addedPhoto = false, faceSwapOptionWithPhoto = false, faceSwapOptionWithLiveImage = false;
double newScaleFactor = (double)scaleFactor / scaleFactor_max * 10;
Size newMinSize = Size(minSize, minSize);
vector<faceMemory> facesList;
string filePath = "";
string getTime(bool frame);
void saveVideo(vector<Mat> images, int width, int height);
Point getCenter(Rect rect);
int getDistance(Point p1, Point p2);
int menu(int number, string filePath);
string openfilename();

int main(void)
{
	Mat controllerFrame = cv::Mat(640, 720, CV_8UC3);
	string WINDOW_NAME = "Controller Frame";
	cvui::init(WINDOW_NAME);

	while (true)
	{
		controllerFrame = cv::Scalar(49, 52, 49);
		cvui::text(controllerFrame, (380 / 2 - 140), 60, "FACE", 2.5, 0xff0000);
		cvui::text(controllerFrame, (380 / 2 - 140), 130, "CHANGER+ ", 2.5, 0xff0000);

		if (cvui::button(controllerFrame, (380 / 2 - 130), 255, "Open camera")) 
		{
			menu(number = 1, "");
			turnOn = true;
			break;
		}
		if (cvui::button(controllerFrame, (380 / 2 + 90), 255, "Open video file"))
		{
			menu(number = 2, openfilename().c_str());
			turnOn = true;
			break;
		}

		cv::imshow(WINDOW_NAME, controllerFrame);

		if (cv::waitKey(20) == 27)
			break;
	}

	if (!turnOn) return 0;
	namedWindow("Frame", WINDOW_AUTOSIZE);
	Mat frame;
	cvui::init(WINDOW_NAME);

	if (!face_cascade.load(face_cascade_nazwa) || !eyes_cascade.load(eyes_cascade_nazwa))
	{
		printf("Error while reading the file for detection \n"); return -1;
	}

	vector<Mat> imagesForRecord;
	int counterFrames = 0;
	string facePhotoName;
	Mat facePhoto, faceToSwapLive;
	while (cap.read(frame)) {

		resize(frame, frame, Size(680, 480), 0.5, 0.5);
		std::vector<Rect> faces;
		Mat frame_gray;

		controllerFrame = cv::Scalar(49, 52, 49);

		if (cvui::button(controllerFrame, 250, 40, "Add photo to change")) 
		{
			addedPhoto = true;
			facePhoto = imread(openfilename().c_str());
		}

		int  yTrackbarJump = 60, yTextJump = 20, yPosLast = 20;
		cvui::text(controllerFrame, 20, yPosLast += yTextJump, "Scaling factor", 0.4, 0xff0000);
		cvui::trackbar(controllerFrame, 20, yPosLast += yTextJump, 150, &scaleFactor, scaleFactor_min, scaleFactor_max);
		cvui::text(controllerFrame, 20, yPosLast += yTrackbarJump, "minNeighbors", 0.4, 0xff0000);
		cvui::trackbar(controllerFrame, 20, yPosLast += yTextJump, 150, &minNeighbors, minNeighbors_min, minNeighbors_max);
		cvui::text(controllerFrame, 20, yPosLast += yTrackbarJump, "minSize", 0.4, 0xff0000);
		cvui::trackbar(controllerFrame, 20, yPosLast += yTextJump, 150, &minSize, minSize_min, minSize_max);
		cvui::text(controllerFrame, 20, yPosLast += yTrackbarJump, "Amount of frames", 0.4, 0xff0000);
		cvui::trackbar(controllerFrame, 20, yPosLast += yTextJump, 150, &frameAmount, frameAmount_min, frameAmount_max);

		yTrackbarJump = 60, yTextJump = 40, yPosLast = 40;
		cvui::checkbox(controllerFrame, 250, yPosLast += yTrackbarJump, "Circle Option", &circleOption);
		cvui::checkbox(controllerFrame, 250, yPosLast += yTextJump, "Black Eyes Option", &blackEyesOption);
		cvui::checkbox(controllerFrame, 250, yPosLast += yTextJump, "Blured Face Option", &bluredFaceOption);
		cvui::checkbox(controllerFrame, 250, yPosLast += yTextJump, "Face Swap Option", &faceSwapOption);
		cvui::checkbox(controllerFrame, 250, yPosLast += yTextJump, "Face Swap Option With Photo", &faceSwapOptionWithPhoto);
		cvui::text(controllerFrame, 250, yPosLast += yTextJump, "Pick one to start recording", 0.4, 0xff0000);

		yPosLast += yTextJump;
		for (int i = 0; i < facesList.size(); i++)
		{
			cv::Mat out = facesList[i].image.clone();
			resize(out, out, Size(40, 40));
			if (!out.empty())
				if (facesList[i].isRecorded)
					rectangle(controllerFrame, Point(-3 + 250 + i * 50, -3 + yPosLast), Point(3 + 40 + 250 + i * 50, 3 + 40 + yPosLast), Scalar(255, 0, 0), 2);

			if (cvui::button(controllerFrame, 250 + i * 50, yPosLast, out, out, out)) {
				facesList[i].isRecorded = !facesList[i].isRecorded;
			}
		}

		
		cvui::text(controllerFrame, 250, yPosLast += yTextJump+20, "Pick one to start swapping", 0.4, 0xff0000);
		yPosLast += yTextJump;
		for (int i = 0; i < facesList.size(); i++)
		{
			cv::Mat out = facesList[i].image.clone();
			resize(out, out, Size(40, 40));
			if (!out.empty())
				if (facesList[i].isToSwap)
					rectangle(controllerFrame, Point(-3 + 250 + i * 50, -3 + yPosLast), Point(3 + 40 + 250 + i * 50, 3 + 40 + yPosLast), Scalar(255, 0, 0), 2);

			if (cvui::button(controllerFrame, 250 + i * 50, yPosLast, out, out, out)) {
				if (faceSwapOptionWithLiveImage)
					facesList[i].isToSwap = !facesList[i].isToSwap;
			}
		}
		cvui::checkbox(controllerFrame, 250, yPosLast += yTextJump + 10, "Face Swap Option With Live", &faceSwapOptionWithLiveImage);

		cvui::update();
		cv::imshow(WINDOW_NAME, controllerFrame);

		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);
		Mat frame_face_copy = frame.clone();
		face_cascade.detectMultiScale(frame_gray, faces, newScaleFactor, minNeighbors, 0, newMinSize);

		for (int i = 0; i < facesList.size(); i++)
			++facesList[i].counter;

		for (int i = 0; i < faces.size(); i++)
		{
			bool faceExists = false;
			Point centerCurrentFace = getCenter(faces[i]);
			for (faceMemory &faceMem : facesList)
			{
				if (getDistance(faceMem.getCenter(), centerCurrentFace) <= 100) {
					faceMem.rect = faces[i];
					faceMem.image = frame(faces[i]).clone();
					faceMem.counter = 0;
					faceExists = true;
					break; 
				}
			}
			if (!faceExists) 
				facesList.push_back(faceMemory(faces[i], frame(faces[i])));
		}

		vector<faceMemory> usableFaces;
		for (int i = 0; i < facesList.size(); i++) 
		{
			bool toDelete = facesList[i].counter == 30;
			if (facesList[i].isRecorded)
				facesList[i].recordedImages.push_back(facesList[i].image);
			if ((facesList[i].recordedImages.size() >= frameAmount) || (toDelete && facesList[i].isRecorded)) {
				facesList[i].isRecorded = false;
				saveVideo(facesList[i].recordedImages, facesList[i].rect.width, facesList[i].rect.height);
				facesList[i].recordedImages.clear();
			}
			if (facesList[i].counter == 0)
				usableFaces.push_back(facesList[i]);
			if (facesList[i].isToSwap)
				faceToSwapLive = facesList[i].image.clone();
			if (toDelete)
				facesList.erase(facesList.begin() + i);
		}

		Mat frame_out = frame.clone();
		for (int i = 0; i < usableFaces.size(); i++)
		{
			if (usableFaces[i].counter > 0)
				continue;
			if (bluredFaceOption)
				GaussianBlur(frame(usableFaces[i].rect), frame_out(usableFaces[i].rect), Size(15, 15), 20);
			if (circleOption)
				circle(frame_out, Point(usableFaces[i].rect.x + usableFaces[i].rect.width / 2, usableFaces[i].rect.y + usableFaces[i].rect.height / 2), cvRound((usableFaces[i].rect.width + usableFaces[i].rect.height)*0.25), Scalar(0, 100, 0), 3);
			if (blackEyesOption)
				rectangle(frame_out, Rect(Point(usableFaces[i].rect.x, usableFaces[i].rect.y + usableFaces[i].rect.height / 2) - Point(0, usableFaces[i].rect.height / 3), Size(usableFaces[i].rect.width, usableFaces[i].rect.height / 3)), Scalar(0, 0, 0), -1);
			if (faceSwapOption && (usableFaces.size() >= 2)) {
				Mat tmpFaceResize = frame(usableFaces[i].rect).clone();
				resize(tmpFaceResize, tmpFaceResize, usableFaces[(i + 1) % usableFaces.size()].rect.size());
				tmpFaceResize.copyTo(frame_out(usableFaces[(i + 1) % usableFaces.size()].rect));
			}
			if (faceSwapOptionWithPhoto && !facePhoto.empty())
			{
				Mat tmpFaceResize = facePhoto.clone();
				resize(tmpFaceResize, tmpFaceResize, usableFaces[i].rect.size());
				tmpFaceResize.copyTo(frame_out(usableFaces[i].rect));
			}
			if (faceSwapOptionWithLiveImage && !faceToSwapLive.empty())
			{
				Mat tmpFaceResize = faceToSwapLive.clone();
				resize(tmpFaceResize, tmpFaceResize, usableFaces[i].rect.size());
				tmpFaceResize.copyTo(frame_out(usableFaces[i].rect));
			}
		}

		imshow("Frame", frame_out);
		klawisz = waitKey(1);

		if (klawisz == 114)  // 'r'
		{
			recordOption = !recordOption;
			if (recordOption) {
				counterFrames = 0;
				imagesForRecord.clear();
			}
			else {
				finishRecord = true;
			}
		}

		if (recordOption || finishRecord)
		{
			++counterFrames;
			imagesForRecord.push_back(frame.clone());
			cout << "is recording" << endl;

			if (counterFrames >= frameAmount || finishRecord) 
			{
				cout << imagesForRecord.size() << endl;
				saveVideo(imagesForRecord, frame.size().width, frame.size().height);
				recordOption = false;
				finishRecord = false;
			}
		}
		if (klawisz == 27)
			break;
	}
	cap.release();
	destroyAllWindows();
	return 0;
}

string openfilename() {
	string fileName = "";
	OPENFILENAME ofn, type;
	char szFileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	if (number == 2)
		ofn.lpstrFilter = _T("Video\0*.avi\0");
	if (addedPhoto)
		ofn.lpstrFilter = _T("JPG\0*.jpg\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileName(&ofn))
		fileName.assign(szFileName);

	ZeroMemory(&ofn, sizeof(ofn));
	return fileName;
}

int menu(int number, string filePath)
{
	if (openCamera)
		number = 1;
	if (openFilm)
		number = 2;

	switch (number) {
	case 1:
		cap.open(0);
		break;
	case 2:
		cap.open(filePath);
		break;
	default:
		cout << "could not be opened";
		return -1;
	}
}
string getTime(bool frame = true)
{
	time_t timeObj;
	time(&timeObj);
	tm *pTime = gmtime(&timeObj);
	char buffer[100];
	sprintf(buffer, "%d-%d-%d-%d-%d-%d", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
	return buffer;
}

void saveVideo(vector<Mat> images, int width, int height)
{
	VideoWriter video("face record - " + getTime(false) + ".avi", VideoWriter::fourcc('D', 'I', 'V', '3'), 27, Size(width, height));
	for (size_t i = 0; i < images.size(); i++)
	{
		resize(images[i], images[i], Size(width, height));
		video.write(images[i]);
	}
	string response = !video.isOpened() ? "Output video could not be opened" : "video saved";
	cout << response << endl;
}

Point getCenter(Rect rect) {
	return Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
}

int getDistance(Point p1, Point p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}
