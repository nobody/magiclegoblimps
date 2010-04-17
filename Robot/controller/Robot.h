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

	bool GetCamConnected() { return camConnected_; }
	bool GetNXTConnected() { return nxtConnected_; }
	bool GetRobotOnline() { return robotOnline_; }

	//should pick which one of these works well enough
	GridLoc* GetObjectLocation(int id);
	GridLoc* GetObjectLocationSimple();

	void centerCameraOnTarget(int deg);
	void setDestination(GridLoc* newD);
	void setNextLoc(GridLoc* newNextLoc);
	void setPath(Path* newPath);
	void updateLocation();
	void setRobotMoving(bool moving) { robotMoving_ = moving; }

	GridLoc* getLocation()		{ return loc; }
	GridLoc* getNextLoc()		{ return nextLoc; }
	GridLoc* getDestination()	{ return dest; }
	Path* getPath()			{ return robPath; }
	int getHeading()			{ return robotHeading_; }
	int getID()					{ return id_; }
	bool getRobotMoving()		{ return robotMoving_; }

	void ExecuteCommand(string command);

	void SetUpdate(int x, int y, int heading, int pan, int battery, int status);
	string newCmd();

	void Update();

private:
	int id_;

	Camera* camera_;
	NXT* nxt_;

	bool nxtConnected_;
	bool camConnected_;

	bool robotOnline_;
	bool robotActive_;
	bool robotMoving_;

	GridLoc* loc;
	GridLoc* nextLoc;
	GridLoc* dest;

	RobotHeading robotHeading_;
	int cameraDirection_;

	int batteryLevel_;
	int status_;

	Path* robPath;
};

#endif