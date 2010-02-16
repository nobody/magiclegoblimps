#include "Camera.h"

#include <iostream>

Camera::Camera(string ip, bool dLinkCam)
{
	ip_ = ip;
	dLinkCam_ = dLinkCam;
}

bool Camera::Connect()
{
	//gives cameras a window for testing
	//need a better way to set this before a connect
	localDisplay_ = true;

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

	if (localDisplay_)
		StartDisplay();

	return true;
}

void Camera::Disconnect()
{
	if (localDisplay_)
		StopDisplay();

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

void Camera::SetLocalDisplay(bool display)
{
	localDisplay_ = display;
}

void Camera::StartDisplay()
{
	displayWindowName_ = "RobotView " + ip_;
	cvNamedWindow(displayWindowName_.c_str(), CV_WINDOW_AUTOSIZE);
}

void Camera::DisplayFrame()
{
	IplImage* frame = cvQueryFrame(capture_);

	if(!frame) 
	{
		fprintf(stderr, "ERROR: Frame is NULL! \n");
		getchar();
		return;
	}		

	cvShowImage(displayWindowName_.c_str(), frame);
}

void Camera::StopDisplay()
{
	cvReleaseCapture(&capture_);
	cvDestroyWindow(displayWindowName_.c_str());
}