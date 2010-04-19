#ifndef TRACKINGOBJECT_H
#define TRACKINGOBJECT_H

#include <iostream>
#include <fstream>
#include <cv.h>

using namespace std;

class TrackingObject
{
public:
	TrackingObject(CvHistogram* hist, CvBox2D originalBox, CvScalar color);

	int GetID() { return id_; }
	void SetID(int id) { id_ = id; }

	float GetArea() 
	{
		return trackBox_.size.width * trackBox_.size.height;
	}
	CvPoint GetCenter();
	CvBox2D GetOriginalBox() { return originalBox_; }
	CvBox2D GetTrackingBox() { return trackBox_; }
	CvScalar GetColor() { return color_; }
	CvRect GetTrackingWindow() { return trackWindow_; }
	CvHistogram* GetHistogram() { return histogram_; }

	void SetTrackingBox(CvBox2D trackBox)
	{
		trackBox_ = trackBox;
	}
	void SetTrackingWindow(CvRect trackWindow)
	{
		trackWindow_ = trackWindow;
	}

	int GetCenteredPercentage(int width);
	float GetQuality(int width);
	int GetSizePercentage();

	int CenterDistanceToDegrees(int width, bool cam);

	static char* BoxToArray(CvBox2D box, int* size);
	static char* HistogramToArray(CvHistogram* hist, int* size);
	static CvBox2D ArrayToBox(char* arr);
	static CvHistogram* ArrayToHistogram(char* arr);

private:
	int id_;

	CvScalar color_;
	CvBox2D originalBox_;
	CvBox2D trackBox_;
	CvRect trackWindow_;
	CvHistogram* histogram_;
};

#endif