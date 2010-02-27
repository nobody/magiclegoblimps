#include "Controller.h"
#include "Tokenizer.h" 

#include <time.h>

Controller::Controller()
{
	timer_ = 0;
}

bool Controller::ConnectToServer(string ip)
{
	//connect to server

	return false;
}

void Controller::TestCommand(string command)
{
	//should translate command from server format into our command format
	//then send it on to the requested robot

	//temporary format (needs discussing with server group)
	//very likely to change
	//[command] [robotnumber] [subcommand] [value]

	vector<string> tokens;
	tokenize(command, tokens, " ");

	if (tokens.size() == 0)
		return;
}

void Controller::AddRobot(Robot* robot)
{
	robots_.push_back(robot);
}

Robot* Controller::GetRobot(int id)
{
	for (int i = 0; i < robots_.size(); i++)
	{
		if (robots_[i]->GetID() == id)
			return robots_[i];
	}
}

void Controller::RemoveRobot(int id)
{
	for (int i = 0; i < robots_.size(); i++)
	{
		if (robots_[i]->GetID() == id)
		{
			robots_[i]->Disconnect();
			delete robots_[i];
			robots_.erase(robots_.begin() + i);
		}
	}
}

vector<Robot*> Controller::GetRobotVector()
{
	return robots_;
}

void Controller::Update()
{
	//check for server message and respond

	time_t seconds = time(NULL);
	float interval = seconds - lastTime_;
	lastTime_ = seconds;

	timer_ += interval;

	if (timer_ > POLL_INTERVAL)
	{
		vector<Robot*>::iterator it;

		for (it = robots_.begin(); it != robots_.end(); it++)
		{
			//need to make sure robot is filling mailbox before using this
			/*
			if ((*it)->GetNXTConnected())
			{
				cout << "Robot " << (*it)->GetID() << " " <<
					(*it)->GetNXT()->ReadMessage((*it)->GetNXT()->IN_MAILBOX) << 
					endl;
			}
			*/
		}

		timer_ = 0;
	}
}