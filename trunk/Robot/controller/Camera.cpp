#include "Camera.h"

#include <iostream>

//static member variables must be redeclared in source
vector<TrackingObject*> Camera::trackableObjects_;

Camera::Camera(string ip, bool dLinkCam)
{
	scanInterval_ = 3;		
	lockTime_ = 1;

	ip_ = ip;
	dLinkCam_ = dLinkCam;

	dLinkUrl_ = "/video.cgi?a=.mjpg";		
	ciscoUrl_ = "/img/mjpeg.cgi?a=.mjpg";

	localDisplay_ = false;
	histDisplay_ = true;		
	showTracking_ = true;		
	locked_ = false;		

	inKey_ = -1;		
	waitingKey_ = false;		

	target_ = -1;		
	
	displayWindowName_ = "";
}

bool Camera::Connect()
{
	capture_ = 0;

	user_ = "admin";

	if (dLinkCam_)
		pass_ = "";
	else
		pass_ = "admin";

	camUrl_ = "http://" + user_ + ":" + pass_ + "@" + ip_;

	if (dLinkCam_)
		camUrl_ += dLinkUrl_;
	else
		camUrl_ += ciscoUrl_;

	//comment this out to disable the camera
	capture_ = cvCreateFileCapture(camUrl_.c_str());

	if (capture_ == 0)
		return false;

	return true;
}

void Camera::Disconnect()
{
	if (localDisplay_)
		StopDisplay();

	cvReleaseCapture(&capture_);
	capture_ = 0;
}

void Camera::SetIP(string ip)
{
	ip_ = ip;
}

void Camera::SetDLinkCam(bool dLinkCam)
{
	dLinkCam_ = dLinkCam;
}

//clean some of this up into member variables/functions
IplImage *image = 0, *hsv = 0, *hue = 0, *mask = 0, *backProject = 0, 
	*histImage = 0;
CvHistogram *hist = 0;
bool selectObject = false;
bool trackObject = false;
bool showHist = false;
bool histCreated = false;
CvPoint origin;
CvRect selection;
CvRect trackWindow;
CvBox2D trackBox;
CvConnectedComp trackComp;
int histDivs = 32;
float histRangesArray[] = {0, 180};
float* histRanges = histRangesArray;
int vMin = 10, vMax = 256, sMin = 30;

void onMouse(int event, int x, int y, int flags, void* param)
{
	if (!image)
		return;

	if (image->origin)
		y = image->height - y;

	if (selectObject)
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = selection.x + CV_IABS(x - origin.x);
		selection.height = selection.y + CV_IABS(y - origin.y);

		selection.x = MAX(selection.x, 0);
		selection.y = MAX(selection.y, 0);
		selection.width = MIN(selection.width, image->width);
		selection.height = MIN(selection.height, image->height);
		selection.width -= selection.x;
		selection.height -= selection.y;
	}

	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = cvPoint(x, y);
		selection = cvRect(x, y, 0, 0);
		selectObject = true;
		trackObject = false;
		histCreated = false;
		break;
	case CV_EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
		{
			trackObject = true;
			trackWindow = selection;
		}
		break;
	}
}

CvScalar hsv2rgb(float hue)
{
	int rgb[3], p, sector;
	static const int sector_data[][3]=
	{{0, 2, 1}, {1, 2, 0}, {1, 0, 2}, {2, 0, 1}, {2, 1, 0}, {0, 1, 2}};
	hue *= 0.033333333333333333333333333333333f;
	sector = cvFloor(hue);
	p = cvRound(255 * (hue - sector));
	p ^= sector & 1 ? 255 : 0;

	rgb[sector_data[sector][0]] = 255;
	rgb[sector_data[sector][1]] = 0;
	rgb[sector_data[sector][2]] = p;

	return cvScalar(rgb[2], rgb[1], rgb[0], 0);
}

int Camera::GetImageWidth()
{
	return image->width;
}

void Camera::StartDisplay()
{
	displayWindowName_ = "RobotView " + ip_;
	cvNamedWindow(displayWindowName_.c_str(), CV_WINDOW_AUTOSIZE);
	cvSetMouseCallback(displayWindowName_.c_str(), onMouse, 0);
	//uncomment these if we need to more finely adjust color values
	//cvCreateTrackbar("VMin", displayWindowName_.c_str(), &vMin, 256, 0);
	//cvCreateTrackbar("VMax", displayWindowName_.c_str(), &vMax, 256, 0);
	//cvCreateTrackbar("SMin", displayWindowName_.c_str(), &sMin, 256, 0);

	histWindowName_ = "HistogramView " + ip_;
	cvNamedWindow(histWindowName_.c_str(), CV_WINDOW_AUTOSIZE);

	localDisplay_ = true;
}

void Camera::DisplayFrame()
{
	if (capture_ == 0 || !image)
		return;

	cvShowImage(displayWindowName_.c_str(), image);
	cvShowImage(histWindowName_.c_str(), histImage);

	if (waitingKey_)
	{
		if (inKey_ == 't')
		{
			if (showTracking_)
				showTracking_ = false;
			else 
				showTracking_ = true;
		}

		//stores the currently tracked (red) object
		if (inKey_ == 's')
		{
			TrackingObject* newObject = new TrackingObject(hist, trackBox);

			newObject->SetTrackingWindow(cvRect(0, 0, 
				image->width, image->height));

			newObject->SetID(GetNextAvailableID());

			trackableObjects_.push_back(newObject);

			cvZero(histImage);

			trackObject = false;
			histCreated = false;
		}

		//displays the centered/distance/quality information
		if (inKey_ == 'c')
		{
			vector<TrackingObject*>::iterator it;

			for (it = visibleObjects_.begin(); it != visibleObjects_.end(); it++)
			{
				float dist = 
					(*it)->GetCenteredPercentage(image->width);

				if (dist < CENTERED_EPSILON && dist > -CENTERED_EPSILON)
				{
					cout << (*it)->GetID() << " is centered." << 
						endl;
				}
				else
				{
					cout << (*it)->GetID() << " is " << dist <<
						"% from the center." << endl;
				}

				cout << (*it)->GetID() << " is size " <<
					(*it)->GetSizePercentage() << "%" << endl;

				cout << (*it)->GetID() << " has quality " <<
					(*it)->GetQuality(image->width) << endl;
			}
		}

		waitingKey_ = false;
	}
}

void Camera::StopDisplay()
{
	localDisplay_ = false;

	cvDestroyWindow(displayWindowName_.c_str());
	cvDestroyWindow(histWindowName_.c_str());
}

void Camera::SendKey(int key)
{
	inKey_ = key;
	waitingKey_ = true;
}

void Camera::Update()
{
	if (!capture_)
		return;

	time_t seconds = time(NULL);
	float interval = (float)seconds - lastTime_;
	lastTime_ = seconds;

	scanTimer_ += interval;

	if (scanTimer_ > scanInterval_)
	{
		Scan();
		scanTimer_ = 0;
	}

	lockTimer_ += interval;

	if (lockTimer_ > lockTime_)
	{
		if (!locked_)
			Lock();

		targetVisible_ = isTargetVisible();
	}

	int binWidth;

	IplImage* frame = cvQueryFrame(capture_);

	if(!frame) 
	{
		cout << "ERROR! " + ip_ + " frame is NULL!" << endl;
		return;
	}		

	if (!image)
    {
        image = cvCreateImage(cvGetSize(frame), 8, 3);
        image->origin = frame->origin;
        hsv = cvCreateImage(cvGetSize(frame), 8, 3);
        hue = cvCreateImage(cvGetSize(frame), 8, 1);
        mask = cvCreateImage(cvGetSize(frame), 8, 1);
        backProject = cvCreateImage(cvGetSize(frame), 8, 1);
        hist = cvCreateHist(1, &histDivs, CV_HIST_ARRAY, &histRanges, 1);
        histImage = cvCreateImage(cvSize(320, 200), 8, 3);
        cvZero(histImage);
    }

    cvCopy(frame, image, 0);
    cvCvtColor(image, hsv, CV_BGR2HSV);

	int _vMin = vMin, _vMax = vMax;

	cvInRangeS(hsv, cvScalar(0, sMin, MIN(_vMin, _vMax), 0),
		cvScalar(180, 256, MAX(_vMin, _vMax), 0), mask);
	cvSplit(hsv, hue, 0, 0, 0);

	if (trackObject)
	{
		if (!histCreated)
		{
			float maxVal = 0.0f;
			cvSetImageROI(hue, selection);
			cvSetImageROI(mask, selection);
			cvCalcHist(&hue, hist, 0, mask);
			cvGetMinMaxHistValue(hist, 0, &maxVal, 0, 0);
			cvConvertScale(hist->bins, hist->bins, 
				maxVal ? 255. / maxVal : 0., 0);
			cvResetImageROI(hue);
			cvResetImageROI(mask);

			cvZero(histImage);
			binWidth = histImage->width / histDivs;
			for (int i = 0; i < histDivs; i++)
			{
				int val = 
					cvRound(cvGetReal1D(hist->bins, i) * 
					histImage->height / 255);
				CvScalar color = hsv2rgb(i * 180.f / histDivs);
				cvRectangle(histImage, cvPoint(i * binWidth, histImage->height),
					cvPoint((i + 1) * binWidth, histImage->height - val),
					color, -1, 8, 0);
			}

			histCreated = true;
		}

		cvCalcBackProject(&hue, backProject, hist);
		cvAnd(backProject, mask, backProject, 0);
		cvCamShift(backProject, trackWindow,
			cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
			&trackComp, &trackBox);
		trackWindow = trackComp.rect;

		if (!image->origin)
			trackBox.angle = -trackBox.angle;

		cvEllipseBox(image, trackBox,
			CV_RGB(255, 0, 0), 3, CV_AA, 0);
	}

	vector<TrackingObject*>::iterator it;

	if (possibleObjects_.size() > 0)
	{
		for (it = possibleObjects_.begin(); it != possibleObjects_.end(); it++)
		{
			cvCalcBackProject(&hue, backProject, 
				(*it)->GetHistogram());
			cvAnd(backProject, mask, backProject, 0);
			trackBox = (*it)->GetTrackingBox();
			cvCamShift(backProject, 
				(*it)->GetTrackingWindow(), 
				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
				&trackComp, &trackBox);
			(*it)->SetTrackingBox(trackBox);
			(*it)->SetTrackingWindow(trackComp.rect);

			if (!image->origin)
				trackBox.angle = -trackBox.angle;
		}
	}

	if (visibleObjects_.size() > 0)
	{
		for (it = visibleObjects_.begin(); it != visibleObjects_.end(); it++)
		{
			cvCalcBackProject(&hue, backProject, 
				(*it)->GetHistogram());
			cvAnd(backProject, mask, backProject, 0);
			trackBox = (*it)->GetTrackingBox();
			cvCamShift(backProject, 
				(*it)->GetTrackingWindow(), 
				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
				&trackComp, &trackBox);
			(*it)->SetTrackingBox(trackBox);
			(*it)->SetTrackingWindow(trackComp.rect);

			if (!image->origin)
				trackBox.angle = -trackBox.angle;

			if (showTracking_)
			{
				if ((*it)->GetID() == target_)
				{
					cvEllipseBox(image, (*it)->GetTrackingBox(),
						CV_RGB(0, 255, 0), 3, CV_AA, 0);
				}
				else
				{
					cvEllipseBox(image, (*it)->GetTrackingBox(),
						CV_RGB(0, 0, 255), 3, CV_AA, 0);
				}
			}

			if ((*it)->GetSizePercentage() > MAX_SIZE ||
				(*it)->GetSizePercentage() < MIN_SIZE)
				visibleObjects_.erase(it);
		}
	}

	if (selectObject && selection.width > 0 && selection.height > 0)
	{
		cvSetImageROI(image, selection);
		cvXorS(image, cvScalarAll(255), image, 0);
		cvResetImageROI(image);
	}
}

int Camera::GetNextAvailableID()
{
	int id = 0;

	vector<TrackingObject*>::iterator it;

	for (it = trackableObjects_.begin(); it != trackableObjects_.end(); it++)
	{
		if ((*it)->GetID() == id)
		{
			id++;
			it = trackableObjects_.begin();
		}
	}

	return id;
}

void Camera::Scan()
{
	//should check if object is already visible before rescanning

	vector<TrackingObject*>::iterator it;

	for (it = possibleObjects_.begin(); it != possibleObjects_.end(); it++)
	{
		(*it)->SetTrackingWindow(cvRect(0, 0, image->width, 
			image->height));
	}

	lockTimer_ = 0;
	locked_ = false;
}

void Camera::Lock()
{

	vector<TrackingObject*>::iterator it;

	for (it = possibleObjects_.begin(); it != possibleObjects_.end(); it++)
	{
		if ((*it)->GetSizePercentage() > MAX_SIZE ||
			(*it)->GetSizePercentage() < MIN_SIZE)
			possibleObjects_.erase(it);
	}

	visibleObjects_ = possibleObjects_;

	locked_ = true;
}

bool Camera::isTargetVisible()
{
	if (target_ == -1)
		return false;

	vector<TrackingObject*>::iterator it;

	for (it = visibleObjects_.begin(); it != visibleObjects_.end(); it++)
	{
		if ((*it)->GetID() == target_)
			return true;
	}

	return false;
}