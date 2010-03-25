#include "Controller.h"

Controller::Controller()
{
	timer_ = 0;
}

bool Controller::ConnectToServer(string ip)
{
	//because we're using Windows we'll use WinSock
	//http://msdn.microsoft.com/en-us/library/ms737591(VS.85).aspx

	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;
	//struct addrinfo *result = NULL, *ptr = NULL, hints;
	char* sendBuf = "This is a test";
	char recvBuf[512];
	int iResult;
	int recvBufLen = 512;

	//iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	//ZeroMemory(&hints, sizeof(hints));

	return false;
}

void Controller::TestCommand(string command)
{
	//should translate command from server format into our command format
	//then send it on to the requested robot

	//temporary format (needs discussing with server group)
	//very likely to change
	//[command]$[robotnumber]$[subcommand]$[value]

	//see main.cpp for (badly implemented) usage

	vector<string> tokens;
	tokenize(command, tokens, "$");

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

	return NULL;
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

void Controller::Disconnect()
{
	for (int i = 0; i < robots_.size(); i++)
	{
		robots_[i]->Disconnect();
		delete robots_[i];
		robots_.erase(robots_.begin() + i);
	}

	//need to delete trackable objects after disconnect
	/*
	for (int i = 0; i < trackableObjects_.size(); i++)
	{
		delete trackableObjects_[i];
		trackableObjects_.erase(trackableObjects_.begin() + i);
	}
	*/
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