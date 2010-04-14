#ifndef ROBOT_H
#define ROBOT_H

#include <string>

#include "Camera.h"
#include "NXT.h"
#include "Tokenizer.h"

using namespace std;

enum RobotHeading
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,
};

class Robot
{
public:
	Robot(int port, string ip, bool dLinkCam);
	~Robot();

	int GetID() { return id_; }
	
	Camera* GetCamera() { return camera_; }
	NXT* GetNXT() { return nxt_; }

	void Connect();
	void Disconnect();

	bool GetNXTConnected() { return nxtConnected_; }
	bool GetCamConnected() { return camConnected_; }
	bool GetRobotOnline() { return robotOnline_; }

	void ExecuteCommand(string command);

	int GetLocationX() { return locationX_; }
	int GetLocationY() { return locationY_; }

	void SetUpdate(int x, int y, int heading, int pan, int battery, int status);

private:
	int id_;

	Camera* camera_;
	NXT* nxt_;

	bool nxtConnected_;
	bool camConnected_;

	bool robotOnline_;
	bool robotActive_;

	int locationX_;
	int locationY_;

	RobotHeading robotHeading_;
	int cameraDirection_;

	int batteryLevel_;
	int status_;
};

#endif