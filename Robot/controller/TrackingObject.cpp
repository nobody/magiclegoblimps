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

float TrackingObject::GetQuality()
{
	//return quality metric
	//some combination of size and distance from center
	//should be between 0 - 1

	return 0;
}

float TrackingObject::GetSizePercentage()
{
	float origArea = originalBox_.size.width * originalBox_.size.height;
	float currentArea = trackBox_.size.width * trackBox_.size.height;

	return currentArea / origArea * 100;
}

int TrackingObject::CenterDistanceToDegrees(int width, bool cam)
{
	int dist = (width / 2) - trackBox_.center.x;

	return dist / width * 45;
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

char* TrackingObject::HistogramToArray(CvHistogram* hist)
{
	cvSave("histogram.xml", hist);

	ifstream file;
	file.open("histogram.xml", ios::in);

	stringstream buffer;
	buffer << file.rdbuf();
	string contents(buffer.str());

	char* arr = new char[contents.length() + 1];
	strcpy(arr, contents.c_str());

	file.close();

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