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

class Camera
{
public:
	Camera(string ip, bool dLinkCam);

	bool Connect();

	void SetIP(string ip);
	void SetDLinkCam(bool dLinkCam);

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
};

#endif