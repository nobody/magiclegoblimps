#include "Robot.h"

Robot::Robot(int port, string ip, bool dLink)
{
	nxt_ = new NXT(port);
	camera_ = new Camera(ip, true);

	if (nxt_->Connect())
		nxtEnabled_ = true;
	if (camera_->Connect())
		camEnabled_ = true;
}