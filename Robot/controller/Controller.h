#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <time.h>
#include <vector>
#include <Windows.h>

#include "Robot.h"
#include "Tokenizer.h" 
#include "TrackingObject.h"

using namespace std;

class Controller
{
public:
	Controller();

	bool ConnectToServer(string ip);

	//local test as if received from server
	void TestCommand(string command);

	void AddRobot(Robot* robot);
	Robot* GetRobot(int id);
	void RemoveRobot(int id);

	vector<Robot*> GetRobotVector();

	void Disconnect();

	void Update();

	static vector<TrackingObject*> GetTrackableObjects() 
	{ 
		return trackableObjects_;
	}

private:
	static const int POLL_INTERVAL = 10;

	vector<Robot*> robots_;

	float timer_;
	time_t lastTime_;

	static vector<TrackingObject*> trackableObjects_;
};

#endif