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