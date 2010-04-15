#include "TrackingObject.h"

TrackingObject::TrackingObject(CvHistogram* hist, CvBox2D originalBox)
{
	int histDivs = 32;
	float histRangesArray[] = {0, 180};
	float *histRanges = histRangesArray;

	histogram_ = cvCreateHist(1, &histDivs, CV_HIST_ARRAY, &histRanges, 1);

	cvCopyHist(hist, &histogram_);
	originalBox_ = originalBox;
	trackBox_ = originalBox;
}

float TrackingObject::GetCenteredPercentage(int width)
{
	float dist = (width / 2) - trackBox_.center.x;

	return dist / width * 200;
}

float TrackingObject::GetSizePercentage()
{
	float origArea = originalBox_.size.width * originalBox_.size.height;
	float currentArea = trackBox_.size.width * trackBox_.size.height;

	return currentArea / origArea * 100;
}

int TrackingObject::GetQuality()
{
	//return quality metric
	//some combination of size and distance from center

	return 0;
}

char* TrackingObject::BoxToArray(CvBox2D box)
{
	int fSize = sizeof(float);
	short size = fSize * 5;
	char* arr = new char[size];

	memcpy(&arr[0], (char*)&box.angle, 4);
	memcpy(&arr[4], (char*)&box.center.x, 4);
	memcpy(&arr[8], (char*)&box.center.y, 4);
	memcpy(&arr[12], (char*)&box.size.height, 4);
	memcpy(&arr[16], (char*)&box.size.width, 4);
	
	return arr;
}

CvBox2D TrackingObject::ArrayToBox(char* arr)
{
	CvBox2D box;
	int fSize = sizeof(float);

	char* chunk = new char[fSize];
	
	chunk = (char*)&box.angle;
	for (int i = 0; i < 4; i++)
		chunk[i] = arr[i];

	chunk = (char*)&box.center.x;
	for (int i = 0; i < 4; i++)
		chunk[i] = arr[i + 4];

	chunk = (char*)&box.center.y;
	for (int i = 0; i < 4; i++)
		chunk[i] = arr[i + 8];

	chunk = (char*)&box.size.height;
	for (int i = 0; i < 4; i++)
		chunk[i] = arr[i + 12];

	chunk = (char*)&box.size.width;
	for (int i = 0; i < 4; i++)
		chunk[i] = arr[i + 16];

	return box;
}

//test
#include <iostream>

char* TrackingObject::HistogramToArray(CvHistogram* hist)
{
	char* arr;

	//histogram
	//=========
	//int type
	//CvArr* bins
	//float tresh[CV_MAX_DIM][2];
	//CvMatND mat

	/*
	std::cout << "type " << histogram_->type << endl;
	std::cout << "bins " << histogram_->bins << endl;
	std::cout << "tresh " << histogram_->thresh << endl;
	std::cout << "mat " << histogram_->mat << endl;
	*/

	return arr;
}

CvHistogram* TrackingObject::ArrayToHistogram(char* arr)
{
	CvHistogram* hist;

	return hist;
}