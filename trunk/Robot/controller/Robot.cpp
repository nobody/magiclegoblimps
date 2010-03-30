#include "Robot.h"

Robot::Robot(int port, string ip, bool dLink)
{
	nxt_ = new NXT(port);
	camera_ = new Camera(ip, dLink);

	id_ = port;

	nxtConnected_ = false;
	camConnected_ = false;
	robotOnline_ = false;
	robotActive_ = false;
}

Robot::~Robot()
{
	delete nxt_;
	delete camera_;
}

void Robot::Connect()
{
	cout << "Bringing Robot " << id_ << " online..." << endl;

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
	if (nxtConnected_)
		nxt_->Disconnect();

	if (camConnected_)
		camera_->Disconnect();

	robotOnline_ = false;

	cout << "Robot " << id_ << " is offline." << endl;
}

void Robot::ExecuteCommand(string command)
{
	//do all the required tasks for each command (mostly sending NXT messages)

	vector<string> tokens;
	tokenize(command, tokens, " ");

	if (tokens.size() == 0)
		return;

	if (tokens[0].compare("move") == 0)
	{
		if (tokens[1].compare("forward") == 0)
		{
			nxt_->SendMessage("move forward " + tokens[2]);
		}
	}

	if (tokens[0].compare("turn") == 0)
	{
		if (tokens[1].compare("camera") == 0)
		{
			nxt_->SendMessage("turn camera " + tokens[2]);
		}

		if (tokens[1].compare("left") == 0)
		{
			nxt_->SendMessage("turn left " + tokens[2]);
		}

		if (tokens[1].compare("right") == 0)
		{
			nxt_->SendMessage("turn right " + tokens[2]);
		}
	}

	if (tokens[0].compare("pan") == 0)
	{
		nxt_->SendMessage("pan " + tokens[1]);
	}

	if (tokens[0].compare("target") == 0)
	{
		camera_->SetTarget(atoi(tokens[1].c_str()));
	}
}