#include "Controller.h"

Controller::Controller()
{
}

bool Controller::ConnectToServer(string ip)
{
	//connect to server

	return false;
}

void Controller::TestCommand(string commmand)
{
	//should translate command from server format into our command format
	//then send it on to the requested robot
}

void Controller::AddRobot(Robot robot)
{
	robots_.push_back(robot);
}

Robot Controller::GetRobot(int id)
{
	for (int i = 0; i < robots_.size(); i++)
	{
		if (robots_.at(i).GetID() == id)
			return robots_.at(i);
	}
}

void Controller::RemoveRobot(int id)
{
	for (int i = 0; i < robots_.size(); i++)
	{
		if (robots_.at(i).GetID() == id)
			robots_.erase(robots_.begin() + i);
	}
}