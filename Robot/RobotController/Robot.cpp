#include "Robot.h"

Robot::Robot(int port, string ip, bool dLink)
{
	nxt_ = new NXT(port);
	camera_ = new Camera(ip, true);

	//not sure if the false conditions (can't connect/error) are working
	//might need to just comment out the one you're not testing
	if (nxt_->Connect())
		nxtEnabled_ = true;
	if (camera_->Connect())
		camEnabled_ = true;
}