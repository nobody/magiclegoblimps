#include "Camera.h"

//static member variables must be redeclared in source
Vector_ts<TrackingObject*> Camera::trackableObjects_;
string Camera::routerIP_;

Camera::Camera(string ip, bool dLinkCam)
{
	scanInterval_ = 2;		
	lockTime_ = 1;

	ip_ = ip;
	dLinkCam_ = dLinkCam;

	dLinkUrl_ = "/video.cgi?a=.mjpg";		
	ciscoUrl_ = "/img/mjpeg.cgi?a=.mjpg";

	vector<string> tokens;
	tokenize(ip, tokens, ":");

	if (tokens.size() > 1)
		port_ = tokens[1];
	else
		port_ = "80";

	image_ = 0;
	capture_ = 0;

	localDisplay_ = false;
	histDisplay_ = true;		
	showTracking_ = true;		
	locked_ = false;	
	targetVisible_ = false;	

	target_ = -1;		

	lastObjectSize_ = 0;
	
	displayWindowName_ = "";

	//wall of member variables/functions
	hsv = 0, hue = 0, mask = 0, backProject = 0, histImage = 0;
	hist = 0;
	selectObject = false;
	trackObject = false;
	showHist = false;
	histCreated = false;
	histDivs = 32;
	histRangesArray[0] = 0;
	histRangesArray[1] = 180;
	histRanges = histRangesArray;
	vMin = 10, vMax = 256, sMin = 30;
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

void Camera::SetRouterIP(string ip)
{
	routerIP_ = ip;
}

string Camera::GetExtURL()
{
	user_ = "admin";

	string url = "http://" + user_ + ":" + pass_ + "@" + routerIP_ + ":" + 
		port_;

	if (dLinkCam_)
		url += dLinkUrl_;
	else 
		url += ciscoUrl_;

	return url;
}

static void onMouse(int event, int x, int y, int flags, void* param)
{
	MouseParams* mp = (MouseParams*)param;

	if (!mp->image)
		return;

	if (mp->image->origin)
		y = mp->image->height - y;

	if (mp->selectObject)
	{
		mp->selection->x = MIN(x, mp->origin->x);
		mp->selection->y = MIN(y, mp->origin->y);
		mp->selection->width = mp->selection->x + CV_IABS(x - mp->origin->x);
		mp->selection->height = mp->selection->y + CV_IABS(y - mp->origin->y);

		mp->selection->x = MAX(mp->selection->x, 0);
		mp->selection->y = MAX(mp->selection->y, 0);
		mp->selection->width = MIN(mp->selection->width, mp->image->width);
		mp->selection->height = MIN(mp->selection->height, mp->image->height);
		mp->selection->width -= mp->selection->x;
		mp->selection->height -= mp->selection->y;
	}

	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		*mp->origin = cvPoint(x, y);
		*mp->selection = cvRect(x, y, 0, 0);
		*mp->selectObject = true;
		*mp->trackObject = false;
		*mp->histCreated = false;
		break;
	case CV_EVENT_LBUTTONUP:
		*mp->selectObject = false;
		if (mp->selection->width > 0 && mp->selection->height > 0)
		{
			*mp->trackObject = true;
			//mp->trackWindow = mp->selection;
			memcpy(mp->trackWindow, mp->selection, sizeof(CvRect));
		}
		break;
	}
}

CvScalar hsv2rgb(float hue)
{
	int rgb[3], p, sector;
	static const int sector_data[][3] = {{0, 2, 1}, {1, 2, 0}, {1, 0, 2}, 
		{2, 0, 1}, {2, 1, 0}, {0, 1, 2}};
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
	return image_->width;
}

void Camera::StartDisplay()
{
	displayWindowName_ = "RobotView " + ip_;
	cvNamedWindow(displayWindowName_.c_str(), CV_WINDOW_AUTOSIZE);
	MouseParams* mp = new MouseParams;
	mp->image = image_;
	mp->selectObject = &selectObject;
	mp->trackObject = &trackObject;
	mp->histCreated = &histCreated;
	mp->origin = &origin;
	mp->selection = &selection;
	mp->trackWindow = &trackWindow;
	cvSetMouseCallback(displayWindowName_.c_str(), onMouse, mp);
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
	if (capture_ == 0 || !image_)
		return;

	cvShowImage(displayWindowName_.c_str(), image_);
	cvShowImage(histWindowName_.c_str(), histImage);
}

void Camera::StopDisplay()
{
	localDisplay_ = false;

	cvDestroyWindow(displayWindowName_.c_str());
	cvDestroyWindow(histWindowName_.c_str());
}

int Camera::SaveObject()
{
	/*
	float maxValue = 0;
	int maxIndex = 0;
	cvGetMinMaxHistValue(hist, NULL, &maxValue, NULL, &maxIndex);
	CvScalar color = CV_RGB(h, s, maxValue);
	*/
	CvScalar color = CV_RGB(0, 0, 0);

	TrackingObject* newObject = new TrackingObject(hist, trackBox, 
		color);

	newObject->SetTrackingWindow(cvRect(0, 0, 
		image_->width, image_->height));

	int id = GetNextAvailableID();

	newObject->SetID(id);

	trackableObjects_.lock();
	trackableObjects_.push_back(newObject);
	trackableObjects_.unlock();

	cvZero(histImage);

	trackObject = false;
	histCreated = false;
	
	return id;
}

void Camera::Update()
{
	if (!capture_)
	{
		Connect();
		return;
	}

	try
	{
	int binWidth;

	IplImage* frame = cvQueryFrame(capture_);

	if(!frame) 
	{
		cout << "ERROR! " + ip_ + " frame is NULL!" << endl;
		cout << "Trying to reconnect..." << endl;
		Connect();
		return;
	}		

	if (!image_)
    {
    	image_ = cvCreateImage(cvGetSize(frame), 8, 3);
    	image_->origin = frame->origin;
    	hsv = cvCreateImage(cvGetSize(frame), 8, 3);
    	hue = cvCreateImage(cvGetSize(frame), 8, 1);
    	mask = cvCreateImage(cvGetSize(frame), 8, 1);
    	backProject = cvCreateImage(cvGetSize(frame), 8, 1);
    	hist = cvCreateHist(1, &histDivs, CV_HIST_ARRAY, &histRanges, 1);
    	histImage = cvCreateImage(cvSize(320, 200), 8, 3);
    	cvZero(histImage);
    }

    cvCopy(frame, image_, 0);
    cvCvtColor(image_, hsv, CV_BGR2HSV);

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
				int val = cvRound(cvGetReal1D(hist->bins, i) * 
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
		cvCamShift(backProject, trackWindow, cvTermCriteria(CV_TERMCRIT_EPS | 
			CV_TERMCRIT_ITER, 10, 1), &trackComp, &trackBox);
		trackWindow = trackComp.rect;

		if (!image_->origin)
			trackBox.angle = -trackBox.angle;

		cvEllipseBox(image_, trackBox, CV_RGB(255, 0, 0), 3, CV_AA, 0);
	}

	vector<TrackingObject*>::iterator it;

	if (possibleObjects_.size() > 0)
	{
		for (it = possibleObjects_.begin(); it != possibleObjects_.end(); it++)
		{
			cvCalcBackProject(&hue, backProject, (*it)->GetHistogram());
			cvAnd(backProject, mask, backProject, 0);
			trackBox = (*it)->GetTrackingBox();
			cvCamShift(backProject, (*it)->GetTrackingWindow(), 
				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1), 
				&trackComp, &trackBox);
			(*it)->SetTrackingBox(trackBox);
			(*it)->SetTrackingWindow(trackComp.rect);

			if (!image_->origin)
				trackBox.angle = -trackBox.angle;
		}
	}

	if (visibleObjects_.size() > 0)
	{
		for (it = visibleObjects_.begin(); it != visibleObjects_.end(); it++)
		{
			cvCalcBackProject(&hue, backProject, (*it)->GetHistogram());
			cvAnd(backProject, mask, backProject, 0);
			trackBox = (*it)->GetTrackingBox();
			cvCamShift(backProject, (*it)->GetTrackingWindow(), 
				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
				&trackComp, &trackBox);
			(*it)->SetTrackingBox(trackBox);
			(*it)->SetTrackingWindow(trackComp.rect);

			if (!image_->origin)
				trackBox.angle = -trackBox.angle;

			if (showTracking_)
			{
				if ((*it)->GetID() == target_)
				{
					cvEllipseBox(image_, trackBox, 
						CV_RGB(0, 255, 0), 3, CV_AA, 0);
				}
				else
				{
					cvEllipseBox(image_, trackBox, 
						CV_RGB(0, 0, 255), 3, CV_AA, 0);
				}
			}

			if ((*it)->GetSizePercentage() > MAX_SIZE || 
				(*it)->GetSizePercentage() < MIN_SIZE)
			{
				visibleObjects_.erase(it);
				if (visibleObjects_.size() == 0)
					break;
				else
					it = visibleObjects_.begin();
			}
		}
	}

	trackableObjects_.readLock();
	if (lastObjectSize_ != trackableObjects_.size())
	{
		lastObjectSize_ = trackableObjects_.size();
		visibleObjects_.clear();
	}
	trackableObjects_.readUnlock();

	if (selectObject && selection.width > 0 && selection.height > 0)
	{
		cvSetImageROI(image_, selection);
		cvXorS(image_, cvScalarAll(255), image_, 0);
		cvResetImageROI(image_);
	}

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
	}
	catch (...)
	{
	}
}

int Camera::GetNextAvailableID()
{
	int id = 1;

	vector<TrackingObject*>::iterator it;

	trackableObjects_.readLock();
	for (it = trackableObjects_.begin(); it != trackableObjects_.end(); it++)
	{
		if ((*it)->GetID() == id)
		{
			id++;
			it = trackableObjects_.begin();
		}
	}
	trackableObjects_.readUnlock();

	return id;
}

void Camera::Scan()
{
	vector<TrackingObject*>::iterator it;

	possibleObjects_.clear();

	if (visibleObjects_.size() > 0)
	{
		bool visible = false;

		trackableObjects_.readLock();
		for (it = trackableObjects_.begin(); it != trackableObjects_.end(); it++)
		{
			vector<TrackingObject*>::iterator jt;

			visible = false;

			for (jt = visibleObjects_.begin(); jt != visibleObjects_.end(); jt++)
			{
				if ((*it)->GetID() == (*jt)->GetID())
				{
					visible = true;
					break;
				}
			}

			if (!visible)
				possibleObjects_.push_back(*it);
		}
		trackableObjects_.readUnlock();
	}
	else
	{
		trackableObjects_.readLock();
		for (it = trackableObjects_.begin(); it != trackableObjects_.end(); it++)
		{
			possibleObjects_.push_back(*it);
		}
		trackableObjects_.readUnlock();
	}

	for (it = possibleObjects_.begin(); it != possibleObjects_.end(); it++)
	{
		(*it)->SetTrackingWindow(cvRect(0, 0, image_->width, image_->height));
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
		{
			possibleObjects_.erase(it);
			if (possibleObjects_.size() == 0)
				break;
			else
				it = possibleObjects_.begin();
		}
	}
	
	for (it = possibleObjects_.begin(); it != possibleObjects_.end(); it++)
	{
		visibleObjects_.push_back(*it);
	}

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

TrackingObject* Camera::GetTrackingObject(int id)
{	
	vector<TrackingObject*>::iterator it;
	trackableObjects_.readLock();
	for (it = trackableObjects_.begin(); it != trackableObjects_.end(); it++)
	{
		if ((*it)->GetID() == id)
		{	
			trackableObjects_.readUnlock();
			return (*it);
		}
	}
	trackableObjects_.readUnlock();

	return NULL;
}

void Camera::RemoveTrackingObject(int id)
{
	vector<TrackingObject*>::iterator it;
	trackableObjects_.lock();
	for (it = trackableObjects_.begin(); it != trackableObjects_.end(); it++)
	{
		if ((*it)->GetID() == id)
		{
			delete (*it);
			trackableObjects_.erase(it);
			break;
		}
	}
	trackableObjects_.unlock();
}