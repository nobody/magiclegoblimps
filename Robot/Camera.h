#ifndef CAMERA_H
#define CAMERA_H

#define CV_NO_BACKWARD_COMPATIBILITY

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <string>

#pragma comment(lib, "libcv200.dll.a")
#pragma comment(lib, "libcvaux200.dll.a")
#pragma comment(lib, "libcxcore200.dll.a")
#pragma comment(lib, "libcxts200.dll.a")
#pragma comment(lib, "libhighgui200.dll.a")
#pragma comment(lib, "libml200.dll.a")

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

	void SetLocalDisplay(bool display);
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
};

#endif