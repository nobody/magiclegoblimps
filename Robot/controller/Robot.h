#ifndef ROBOT_H
#define ROBOT_H

#include <string>
#include <queue>
#include <sstream>
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

	void centerCameraOnTarget();
	void setDestination(GridLoc* newD);
	void setNextLoc(GridLoc* newNextLoc);
	void setPath(Path* newPath);
	void setRobotMoving(bool moving) { robotMoving_ = moving; }
	void setHasDest(bool dest) { hasDest = dest; }
	void setHeading(RobotHeading head) { robotHeading_ = head; }
	void updateLocation();

	GridLoc* getLocation()		{ return loc; }
	GridLoc* getNextLoc()		{ return nextLoc; }
	GridLoc* getDestination()	{ return dest; }
	Path* getPath()				{ return robPath; }
	int getHeading()			{ return robotHeading_; }
	int getID()					{ return id_; }
	bool getRobotMoving()		{ return robotMoving_; }
	int getStatus()				{ return status_; }
	int getHasPath()			{ return hasPath; }
	int getHasDest()			{ return hasDest; }
	int getCamDir()				{ return cameraDirection_; }
	int getBatt()				{ return batteryLevel_; }

	void SetCameraPanning(bool panning) { cameraPanning_ = panning; }

	void setSearchLoc(int x, int y);
	GridLoc* getSearchLoc()		{ return searchLoc; }

	void ExecuteCommand(string command);

	void SetUpdateMovement(int x, int y, int heading, int battery, int status);
	void SetUpdatePan(int pan);
	string newCmd();

	HANDLE GetSemaphore() { return updateSemaphore_; }

	bool GetRunning() { return running_; }
	void StartRunning() { running_ = true; }
	void StopRunning() { running_ = false; }

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
	bool cameraPanning_;

	GridLoc* loc;
	GridLoc* nextLoc;
	GridLoc* dest;
	bool hasDest;

	GridLoc* searchLoc;

	RobotHeading robotHeading_;
	int cameraDirection_;

	int batteryLevel_;
	int status_;

	Path* robPath;
	bool hasPath;

	float timer_;
	time_t lastTime_;
	int panTime_;

	HANDLE updateSemaphore_;

	bool running_;
};

#endif