#ifndef TRACKINGOBJECT_H
#define TRACKINGOBJECT_H

#include <cv.h>

using namespace std;

class TrackingObject
{
public:
	TrackingObject(CvHistogram* hist, CvBox2D originalBox);

	int GetID() { return id_; }
	void SetID(int id) { id_ = id; }

	CvScalar GetColor();
	float GetArea() 
	{
		return trackBox_.size.width * trackBox_.size.height;
	}
	CvPoint GetCenter();
	CvBox2D GetOriginalBox() { return originalBox_; }
	CvBox2D GetTrackingBox() { return trackBox_; }
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

	float GetCenteredPercentage(int width);
	float GetSizePercentage();

	int GetQuality();

	static char* BoxToArray(CvBox2D box);
	static CvBox2D ArrayToBox(char* arr);
	static char* HistogramToArray(CvHistogram* hist);
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