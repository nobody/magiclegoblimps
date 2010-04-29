#include "TrackingObject.h"

TrackingObject::TrackingObject(CvHistogram* hist, CvBox2D originalBox, CvScalar color)
{
	int histDivs = 32;
	float histRangesArray[] = {0, 180};
	float *histRanges = histRangesArray;

	histogram_ = cvCreateHist(1, &histDivs, CV_HIST_ARRAY, &histRanges, 1);

	cvCopyHist(hist, &histogram_);
	originalBox_ = originalBox;
	trackBox_ = originalBox;

	color_ = color;
}

int TrackingObject::GetCenteredPercentage(int width)
{
	float dist = (width / 2) - trackBox_.center.x;

	return dist / width * 200;
}

float TrackingObject::GetQuality(int width)
{
	int size = GetSizePercentage();
	int centered = GetCenteredPercentage(width);

	if (centered > 0)
	{
		centered -= 100;
		centered = -centered;
	}
	else if (centered < 0)
	{
		centered += 100;
	}

	if (size > 100)
	{
		size = 200 - size;
	}

	if (size < 0)
	{
		size = 0;
	}

	return (centered + size) / 200.0f;
}

int TrackingObject::GetSizePercentage()
{
	float origArea = originalBox_.size.width * originalBox_.size.height;
	float currentArea = trackBox_.size.width * trackBox_.size.height;

	return currentArea / origArea * 100;
}

int TrackingObject::CenterDistanceToDegrees(int width, bool cam)
{
	float centered = GetCenteredPercentage(width);
	centered /= 100.0f;

	centered = -centered;

	//epsilon
	if (abs(centered) > 0.15f)
	{
		if (cam == true)
			return centered * 45.0f;
		else 
			return centered * 60.0f;
	}
	else
		return 0;
}

char* TrackingObject::BoxToArray(CvBox2D box, int* size)
{
	int fSize = sizeof(float);
	short ssize = fSize * 5;
	char* arr = new char[ssize];

	memcpy(&arr[0], (char*)&box.angle, 4);
	memcpy(&arr[4], (char*)&box.center.x, 4);
	memcpy(&arr[8], (char*)&box.center.y, 4);
	memcpy(&arr[12], (char*)&box.size.height, 4);
	memcpy(&arr[16], (char*)&box.size.width, 4);

	*size = ssize;
	
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

char* TrackingObject::HistogramToArray(CvHistogram* hist, int* size)
{
	cvSave("histogram.xml", hist);

	ifstream file;
	file.open("histogram.xml", ios::in);

	stringstream buffer;
	buffer << file.rdbuf();
	string contents(buffer.str());

	int ssize = contents.length() + 1;

	char* arr = new char[ssize];
	strcpy(arr, contents.c_str());

	file.close();

	*size = ssize;

	return arr;
}

CvHistogram* TrackingObject::ArrayToHistogram(char* arr)
{
	CvHistogram* hist;

	ofstream file;
	file.open("histogram.xml", ios::out |  ios::trunc);
	file << arr;
	file.close();

	hist = (CvHistogram*)cvLoad("histogram.xml");

	return hist;
}