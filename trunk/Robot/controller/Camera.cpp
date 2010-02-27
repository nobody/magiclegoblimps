#include "Camera.h"

#include <iostream>

Camera::Camera(string ip, bool dLinkCam)
{
	ip_ = ip;
	dLinkCam_ = dLinkCam;

	localDisplay_ = false;
}

bool Camera::Connect()
{
	capture_ = 0;

	dLinkUrl_ = "/video.cgi?a=.mjpg";
	ciscoUrl_ = "/img/mjpeg.cgi?a=.mjpg";

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
}

void Camera::SetIP(string ip)
{
	ip_ = ip;
}

void Camera::SetDLinkCam(bool dLinkCam)
{
	dLinkCam_ = dLinkCam;
}

void Camera::StartDisplay()
{
	localDisplay_ = true;

	displayWindowName_ = "RobotView " + ip_;
	cvNamedWindow(displayWindowName_.c_str(), CV_WINDOW_AUTOSIZE);
}

void Camera::DisplayFrame()
{
	if (capture_ == 0 || !localDisplay_)
		return;

	IplImage* frame = cvQueryFrame(capture_);

	if(!frame) 
	{
		cout << "ERROR! " + displayWindowName_ + " Frame is NULL!" << endl;
		return;
	}		

	cvShowImage(displayWindowName_.c_str(), frame);
}

void Camera::StopDisplay()
{
	cvDestroyWindow(displayWindowName_.c_str());

	localDisplay_ = false;
}