#ifndef CAMERA_H
#define CAMERA_H

#define CV_NO_BACKWARD_COMPATIBILITY
#define HIGHGUI_NO_BACKWARD_COMPATIBILITY

#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include <cv.h>
#include <highgui.h>

//#include "Controller.h"
#include "TrackingObject.h"

#ifdef _DEBUG
#pragma comment(lib, "cv200d.lib")
#pragma comment(lib, "cvaux200d.lib")
#pragma comment(lib, "cxcore200d.lib")
#pragma comment(lib, "cxts200d.lib")
#pragma comment(lib, "highgui200d.lib")
#pragma comment(lib, "ml200d.lib")
#pragma comment(lib, "opencv_ffmpeg200d.lib")
#else
#pragma comment(lib, "cv200.lib")
#pragma comment(lib, "cvaux200.lib")
#pragma comment(lib, "cxcore200.lib")
#pragma comment(lib, "cxts200.lib")
#pragma comment(lib, "highgui200.lib")
#pragma comment(lib, "ml200.lib")
#pragma comment(lib, "opencv_ffmpeg200.lib")
#endif

using namespace std;

class Camera
{
public:
	Camera(string ip, bool dLinkCam);

	bool Connect();
	void Disconnect();

	void SetIP(string ip);
	void SetDLinkCam(bool dLinkCam);

	bool GetLocalDisplay() { return localDisplay_; }

	void StartDisplay();
	void DisplayFrame();
	void StopDisplay();
	
	void SetTarget(int id) { target_ = id; }

	void SendKey(int key);

	void Update();

private:
	static const int SCAN_INTERVAL = 3;
	static const int LOCK_TIME = 1;

	static const int MIN_AREA = 850;
	static const int MAX_AREA = 115000;

	static const int CENTERED_EPSILON = 15;

	string dLinkUrl_;
	string ciscoUrl_;

	float scanTimer_;
	float lockTimer_;
	time_t lastTime_;

	string ip_;
	bool dLinkCam_;
	string user_;
	string pass_;

	CvCapture* capture_;
	IplImage* image_;

	bool localDisplay_;
	string displayWindowName_;
	bool histDisplay_;
	string histWindowName_;

	int inKey_;
	bool waitingKey_;

	bool showTracking_;
	vector<TrackingObject*> trackableObjects_;
	vector<TrackingObject*> possibleObjects_;
	vector<TrackingObject*> visibleObjects_;

	int target_;

	bool locked_;

	void Scan();
	void Lock();
};

#endif