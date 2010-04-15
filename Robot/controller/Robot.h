#ifndef ROBOT_H
#define ROBOT_H

#include <string>
#include <queue>
#include "Camera.h"
#include "NXT.h"
#include "Tokenizer.h"
#include "GridLoc.h"
#include "Path.h"

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

	void setDestination(GridLoc* newD);
	void updateLocation();
	void setNextLoc(GridLoc* newNextLoc);
	void setPath(Path* newPath);

	GridLoc* getLocation()		{ return loc; }
	GridLoc* getNextLoc()		{ return nextLoc; }
	GridLoc* getDestination()	{ return dest; }
	Path* getPath()				{ return robPath; }

	void ExecuteCommand(string command);

	void SetUpdate(int x, int y, int heading, int pan, int battery, int status);

private:
	int id_;

	Camera* camera_;
	NXT* nxt_;

	bool nxtConnected_;
	bool camConnected_;

	bool robotOnline_;
	bool robotActive_;

	GridLoc* loc;
	GridLoc* nextLoc;
	GridLoc* dest;

	float locationX_;
	float locationY_;

	RobotHeading robotHeading_;
	int cameraDirection_;

	int batteryLevel_;
	int status_;

	Path* robPath;
};

#endif