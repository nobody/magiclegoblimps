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

#include "Tokenizer.h"

#include "TrackingObject.h"
#include "Server/Vector_ts.h"

//#ifdef _DEBUG
//#pragma comment(lib, "cv210d.lib")
//#pragma comment(lib, "cvaux210d.lib")
//#pragma comment(lib, "cxcore210d.lib")
//#pragma comment(lib, "cxts210d.lib")
//#pragma comment(lib, "highgui210d.lib")
//#pragma comment(lib, "ml210d.lib")
//#pragma comment(lib, "opencv_ffmpeg210d.lib")
//#else
//#pragma comment(lib, "cv210.lib")
//#pragma comment(lib, "cvaux210.lib")
//#pragma comment(lib, "cxcore210.lib")
//#pragma comment(lib, "cxts210.lib")
//#pragma comment(lib, "highgui210.lib")
//#pragma comment(lib, "ml210.lib")
//#pragma comment(lib, "opencv_ffmpeg210.lib")
//#endif

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

struct MouseParams
{
	IplImage* image;
	bool* selectObject;
	bool* trackObject;
	bool* histCreated;
	CvPoint* origin;
	CvRect* selection;
	CvRect* trackWindow;
};

class Camera
{
public:
	Camera(string ip, bool dLinkCam);

	bool Connect();
	void Disconnect();

	string GetIP() { return ip_; }
	void SetIP(string ip);
	void SetDLinkCam(bool dLinkCam);

	static void SetRouterIP(string ip);
	string GetExtURL();

	bool GetDLinkCam() { return dLinkCam_; }
	string GetVideoURL() { return camUrl_; }

	bool GetLocalDisplay() { return localDisplay_; }

	int GetImageWidth();

	void StartDisplay();
	void DisplayFrame();
	void StopDisplay();

	int SaveObject();

	void SetTarget(int id)	{ target_ = id; }
	int GetTargetID()		{ return target_; }
	bool GetTargetVisible()	{ return targetVisible_; }

	static Vector_ts<TrackingObject*>* GetTrackableObjects()
	{
		return &trackableObjects_;
	}
	static TrackingObject* GetTrackingObject(int id);
	static void RemoveTrackingObject(int id);

	vector<TrackingObject*> GetVisibleObjects()
	{
		return visibleObjects_;
	}

	void Update();

private:
	static const int MIN_SIZE = 10;
	static const int MAX_SIZE = 300;
	static const int CENTERED_EPSILON = 15;
	
	float scanInterval_;
	float lockTime_;

	string dLinkUrl_;
	string ciscoUrl_;

	float scanTimer_;
	float lockTimer_;
	time_t lastTime_;

	static string routerIP_;

	string ip_;
	bool dLinkCam_;
	string user_;
	string pass_;
	string camUrl_;
	string port_;

	//aren't currently used in code, but should replace local versions
	CvCapture* capture_;
	IplImage* image_;

	bool localDisplay_;
	string displayWindowName_;
	bool histDisplay_;
	string histWindowName_;

	int GetNextAvailableID();

	static Vector_ts<TrackingObject*> trackableObjects_;

	bool showTracking_;
	vector<TrackingObject*> possibleObjects_;
	vector<TrackingObject*> visibleObjects_;
	bool targetVisible_;

	bool isTargetVisible();

	int target_;

	bool locked_;

	int lastObjectSize_;

	void Scan();
	void Lock();

	//wall of member variables/functions
	IplImage *hsv, *hue, *mask, *backProject, *histImage;
	CvHistogram *hist;
	bool selectObject;
	bool trackObject;
	bool showHist;
	bool histCreated;
	CvPoint origin;
	CvRect selection;
	CvRect trackWindow;
	CvBox2D trackBox;
	CvConnectedComp trackComp;
	int histDivs;
	float histRangesArray[2];
	float* histRanges;
	int vMin, vMax, sMin;
};

#endif