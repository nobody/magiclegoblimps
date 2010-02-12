#ifndef ROBOT_H
#define ROBOT_H

#include <string>
#include "Camera.h"
#include "NXT.h"

using namespace std;

class Robot
{
public:
	Robot(int port, string ip, bool dLinkCam);
	
	Camera* GetCamera() { return camera_; }
	NXT* GetNXT() { return nxt_; }

	bool GetNXTEnabled() { return nxtEnabled_; }
	void SetNXTEnabled(bool enabled);
	bool GetCamEnabled() { return camEnabled_; }
	void SetCamEnabled(bool enabled);

private:
	Camera* camera_;
	NXT* nxt_;

	bool nxtEnabled_;
	bool camEnabled_;

	bool robotOnline_;
};

#endif