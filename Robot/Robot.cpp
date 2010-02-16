#include "Robot.h"

Robot::Robot(int port, string ip, bool dLink)
{
	nxt_ = new NXT(port);
	camera_ = new Camera(ip, true);

	id_ = port;

	cout << "Bringing Robot " << id_ << "online..." << endl;

	Robot::Connect();
}

void Robot::Connect()
{
	//not sure if the false conditions (can't connect/error) are working
	//might need to just comment out the one you're not testing
	if (!nxtConnected_)
	{
		if (nxt_->Connect())
		{
			nxtConnected_ = true;
			cout << "Robot " << id_ << " NXT connected." << endl;
		}
		else
			cout << "Robot " << id_ << " NXT failed to connect." << endl;
	}
	else
		cout << "Robot " << id_ << " NXT already connected." << endl;

	if (!camConnected_)
	{
		if (camera_->Connect())
		{
			camConnected_ = true;
			cout << "Robot " << id_ << " camera connected." << endl;
		}
		else
			cout << "Robot " << id_ << " camera failed to connect." << endl;
	}
	else
		cout << "Robot " << id_ << " camera already connected." << endl;

	if (nxtConnected_ && camConnected_)
	{
		robotOnline_ = true;
		cout << "Robot " << id_ << " online." << endl;
	}
	else
	{
		robotOnline_ = false;
		cout << "Robot " << id_ << " not fully operational." << endl;
	}
}

void Robot::Disconnect()
{
	nxt_->Disconnect();
	camera_->Disconnect();

	robotOnline_ = false;

	cout << "Robot " << id_ << "is offline." << endl;
}

void Robot::ExecuteCommand(string command)
{
	//do all the required tasks for each command (mostly sending NXT messages)
}