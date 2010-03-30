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