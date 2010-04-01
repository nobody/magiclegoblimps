#include "Camera.h"

//static member variables must be redeclared in source
int Camera::nextObject_;
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
	pass_ = "admin";

	string camUrl;

	camUrl = "http://" + user_ + ":" + pass_ + "@" + ip_;

	if (dLinkCam_)
		camUrl += dLinkUrl_;
	else
		camUrl += ciscoUrl_;

	capture_ = cvCreateFileCapture(camUrl.c_str());

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

	//needs to be left to the controller
	for (int i = 0; i < trackableObjects_.size(); i++)
	{
		delete trackableObjects_[i];
		trackableObjects_.erase(trackableObjects_.begin() + i);
	}
}

void Camera::SetIP(string ip)
{
	ip_ = ip;
}

void Camera::SetDLinkCam(bool dLinkCam)
{
	dLinkCam_ = dLinkCam;
}

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

void Camera::StartDisplay()
{
	displayWindowName_ = "RobotView " + ip_;
	cvNamedWindow(displayWindowName_.c_str(), CV_WINDOW_AUTOSIZE);
	cvSetMouseCallback(displayWindowName_.c_str(), onMouse, 0);
	//cvCreateTrackbar("VMin", displayWindowName_.c_str(), &vMin, 256, 0);
	//cvCreateTrackbar("VMax", displayWindowName_.c_str(), &vMax, 256, 0);
	//cvCreateTrackbar("SMin", displayWindowName_.c_str(), &sMin, 256, 0);

	histWindowName_ = "HistogramView " + ip_;
	cvNamedWindow(histWindowName_.c_str(), CV_WINDOW_AUTOSIZE);

	localDisplay_ = true;
}

void Camera::DisplayFrame()
{
	if (!capture_ || !image)
		return;

	int binWidth;

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

			newObject->SetID(nextObject_);
			nextObject_++;

			trackableObjects_.push_back(newObject);

			cvZero(histImage);

			trackObject = false;
			histCreated = false;
		}

		//displays the centered/distance information
		if (inKey_ == 'c')
		{
			for (int i = 0; i < visibleObjects_.size(); i++)
			{
				float dist = 
					visibleObjects_[i]->GetCenteredPercentage(image->width);

				if (dist < CENTERED_EPSILON && dist > -CENTERED_EPSILON)
				{
					cout << visibleObjects_[i]->GetID() << " is centered." << 
						endl;
				}
				else
				{
					cout << visibleObjects_[i]->GetID() << " is " << dist <<
						"% from the center." << endl;
				}

				cout << visibleObjects_[i]->GetID() << " is size " <<
					visibleObjects_[i]->GetSizePercentage() << "%" << endl;
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
	float interval = seconds - lastTime_;
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

	if (possibleObjects_.size() > 0)
	{
		for (int i = 0; i < possibleObjects_.size(); i++)
		{
			cvCalcBackProject(&hue, backProject, 
				possibleObjects_[i]->GetHistogram());
			cvAnd(backProject, mask, backProject, 0);
			trackBox = possibleObjects_[i]->GetTrackingBox();
			cvCamShift(backProject, 
				possibleObjects_[i]->GetTrackingWindow(), 
				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
				&trackComp, &trackBox);
			possibleObjects_[i]->SetTrackingBox(trackBox);
			possibleObjects_[i]->SetTrackingWindow(trackComp.rect);

			if (!image->origin)
				trackBox.angle = -trackBox.angle;
		}
	}

	if (visibleObjects_.size() > 0)
	{
		for (int i = 0; i < visibleObjects_.size(); i++)
		{
			cvCalcBackProject(&hue, backProject, 
				visibleObjects_[i]->GetHistogram());
			cvAnd(backProject, mask, backProject, 0);
			trackBox = visibleObjects_[i]->GetTrackingBox();
			cvCamShift(backProject, 
				visibleObjects_[i]->GetTrackingWindow(), 
				cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
				&trackComp, &trackBox);
			visibleObjects_[i]->SetTrackingBox(trackBox);
			visibleObjects_[i]->SetTrackingWindow(trackComp.rect);

			if (!image->origin)
				trackBox.angle = -trackBox.angle;

			if (showTracking_)
			{
				if (visibleObjects_[i]->GetID() == target_)
				{
					cvEllipseBox(image, visibleObjects_[i]->GetTrackingBox(),
						CV_RGB(0, 255, 0), 3, CV_AA, 0);
				}
				else
				{
					cvEllipseBox(image, visibleObjects_[i]->GetTrackingBox(),
						CV_RGB(0, 0, 255), 3, CV_AA, 0);
				}
			}

			if (visibleObjects_[i]->GetSizePercentage() > MAX_SIZE ||
				visibleObjects_[i]->GetSizePercentage() < MIN_SIZE)
				visibleObjects_.erase(visibleObjects_.begin() + i);
		}
	}

	if (selectObject && selection.width > 0 && selection.height > 0)
	{
		cvSetImageROI(image, selection);
		cvXorS(image, cvScalarAll(255), image, 0);
		cvResetImageROI(image);
	}
}

void Camera::Scan()
{
	possibleObjects_ = trackableObjects_;

	//should check if object is already visible before rescanning

	for (int i = 0; i < possibleObjects_.size(); i++)
	{
		possibleObjects_[i]->SetTrackingWindow(cvRect(0, 0, image->width, 
			image->height));
	}

	lockTimer_ = 0;
	locked_ = false;
}

void Camera::Lock()
{
	for (int i = 0; i < possibleObjects_.size(); i++)
	{
		if (possibleObjects_[i]->GetSizePercentage() > MAX_SIZE ||
			possibleObjects_[i]->GetSizePercentage() < MIN_SIZE)
			possibleObjects_.erase(possibleObjects_.begin() + i);
	}

	visibleObjects_ = possibleObjects_;

	locked_ = true;
}