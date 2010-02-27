#ifndef CAMERA_H
#define CAMERA_H

#define CV_NO_BACKWARD_COMPATIBILITY
#define HIGHGUI_NO_BACKWARD_COMPATIBILITY

#include <cv.h>
#include <highgui.h>
#include <string>

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

enum ObjectColor
{
	RED = 0,
	GREEN = 1,
	BLUE = 2,
	CYAN = 3,
	YELLOW = 4,
	MAGENTA = 5,
};

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

private:
	string dLinkUrl_;
	string ciscoUrl_;

	string ip_;
	bool dLinkCam_;
	string user_;
	string pass_;

	CvCapture* capture_;

	bool localDisplay_;
	string displayWindowName_;

	ObjectColor trackingObject_;
	vector<ObjectColor> visibleObjects_;
};

#endif