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

	batteryLevel_ = 0;
	status_ = 0;

	loc = new GridLoc();
	dest = new GridLoc();

	cout << "Bringing Robot " << id_ << " online..." << endl;

	Robot::Connect();
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
	vector<string> tokens;
	tokenize(command, tokens, " ");

	if (tokens.size() == 0)
		return;
	
	if (tokens[0].compare("target") == 0)
	{
		camera_->SetTarget(atoi(tokens[1].c_str()));
	}
	else
	{
		//command
		//=======
		//pan x
		//stop
		//left
		//right
		//turnaround
		//calibrate
		//forward
		//forward x

		nxt_->SendMessage(command);
	}
}

void Robot::SetUpdate(int x, int y, int heading, int pan, int battery, 
	int status)
{
	loc->setX(x);
	loc->setY(y);
	if (heading = 0)
		robotHeading_ = NORTH;
	else if (heading = 1)
		robotHeading_ = EAST;
	else if (heading = 2)
		robotHeading_ = SOUTH;
	else if (heading = 3)
		robotHeading_ = WEST;
	cameraDirection_ = pan;
	batteryLevel_ = battery;
	status_ = status;

	//left shift status

	//status
	//======
	//idle 1
	//destination 2
	//line follow 4
	//intersection 8
	//turn left 16
	//turn right 32
	//turnaround 64
	//sonar block 128
	//calibrate 256
	//pan 512
	//stop 1024
}

void Robot::setDestination(GridLoc* newD)
{
	delete dest;
	dest = newD;
}

void Robot::updateLocation()
{
	//delete loc;
	loc = nextLoc;
	setNextLoc(robPath->advPath());
}

void Robot::setNextLoc(GridLoc* newNextLoc)
{
	//delete nextLoc;
	nextLoc = newNextLoc;
}

void Robot::setPath(Path* newPath)
{
	if(robPath)
		delete robPath;
	robPath = newPath;
}