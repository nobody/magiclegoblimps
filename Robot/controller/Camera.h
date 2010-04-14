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

	string GetIP() { return ip_; }
	void SetIP(string ip);
	void SetDLinkCam(bool dLinkCam);

	bool GetDLinkCam() { return dLinkCam_; }
	string GetVideoURL() { return camUrl_; }

	bool GetLocalDisplay() { return localDisplay_; }

	void StartDisplay();
	void DisplayFrame();
	void StopDisplay();
	
	void SetTarget(int id) { target_ = id; }

	void SendKey(int key);

	static vector<TrackingObject*> GetTrackableObjects()
	{
		return trackableObjects_;
	}

	vector<TrackingObject*> GetVisibleObjects()
	{
		return visibleObjects_;
	}

	void Update();

private:
	static const int MIN_SIZE = 5;
	static const int MAX_SIZE = 400;
	static const int CENTERED_EPSILON = 15;
	
	static int nextObject_;

	float scanInterval_;
	float lockTime_;

	string dLinkUrl_;
	string ciscoUrl_;

	float scanTimer_;
	float lockTimer_;
	time_t lastTime_;

	string ip_;
	bool dLinkCam_;
	string user_;
	string pass_;
	string camUrl_;

	CvCapture* capture_;
	IplImage* image_;

	bool localDisplay_;
	string displayWindowName_;
	bool histDisplay_;
	string histWindowName_;

	int inKey_;
	bool waitingKey_;

	int GetNextAvailableID();

	static vector<TrackingObject*> trackableObjects_;

	bool showTracking_;
	vector<TrackingObject*> possibleObjects_;
	vector<TrackingObject*> visibleObjects_;

	int target_;

	bool locked_;

	void Scan();
	void Lock();
};

#endif