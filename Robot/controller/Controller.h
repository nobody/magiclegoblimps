#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>

#include "Robot.h"

using namespace std;

class Controller
{
public:
	Controller();

	bool ConnectToServer(string ip);

	//local test as if received from server
	void TestCommand(string command);

	void AddRobot(Robot robot);
	Robot GetRobot(int id);
	void RemoveRobot(int id);

private:
	vector<Robot> robots_;
};

#endif