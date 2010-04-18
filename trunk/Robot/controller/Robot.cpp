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
	robotMoving_ = false;

	batteryLevel_ = 0;
	status_ = 0;

	loc = new GridLoc();
	dest = new GridLoc();
	robotHeading_ = NORTH;
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
		else cout << "Robot " << id_ << " NXT failed to connect." << endl;
	}
	else cout << "Robot " << id_ << " NXT already connected." << endl;

	if (!camConnected_)
	{
		if (camera_->Connect())
		{
			camConnected_ = true;
			cout << "Robot " << id_ << " camera connected." << endl;
		}
		else cout << "Robot " << id_ << " camera failed to connect." << endl;
	}
	else cout << "Robot " << id_ << " camera already connected." << endl;

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

GridLoc* Robot::GetObjectLocation(int id)
{
	GridLoc* objLoc = new GridLoc();
	int center = 0;

	vector<TrackingObject*>::iterator it;

	for (it = camera_->GetVisibleObjects().begin(); it != camera_->GetVisibleObjects().end(); it++)
	{
		if ((*it)->GetID() == id)
			center = (*it)->CenterDistanceToDegrees(camera_->GetImageWidth(), camera_->GetDLinkCam());
	}

	int objectDirection = cameraDirection_ + center;

	if (objectDirection >= 0 && objectDirection < 90)
	{
		objLoc->setX(loc->getX());
		objLoc->setY(loc->getY());
	}
	else if (objectDirection >= 90 && objectDirection < 180)
	{
		objLoc->setX(loc->getX());
		objLoc->setY(loc->getY() - 1);
	}
	else if (objectDirection >= 180 && objectDirection < 270)
	{
		objLoc->setX(loc->getX() - 1);
		objLoc->setY(loc->getY() - 1);
	}
	else if (objectDirection >= 270 && objectDirection < 360)
	{
		objLoc->setX(loc->getX() - 1);
		objLoc->setY(loc->getY());
	}

	return objLoc;
}

GridLoc* Robot::GetObjectLocationSimple()
{
	GridLoc* objLoc = new GridLoc();

	if (cameraDirection_ >= 0 && cameraDirection_ < 90)
	{
		objLoc->setX(loc->getX());
		objLoc->setY(loc->getY());
	}
	else if (cameraDirection_ >= 90 && cameraDirection_ < 180)
	{
		objLoc->setX(loc->getX());
		objLoc->setY(loc->getY() - 1);
	}
	else if (cameraDirection_ >= 180 && cameraDirection_ < 270)
	{
		objLoc->setX(loc->getX() - 1);
		objLoc->setY(loc->getY() - 1);
	}
	else if (cameraDirection_ >= 270 && cameraDirection_ < 360)
	{
		objLoc->setX(loc->getX() - 1);
		objLoc->setY(loc->getY());
	}

	return objLoc;
}

void Robot::ExecuteCommand(string command)
{
	vector<string> tokens;
	tokenize(command, tokens, " ");

	if (tokens.size() == 0)
		return;
	
	if (tokens[0].compare("target") == 0)
	{
		//do what you need to with these
		int id = atoi(tokens[1].c_str());
		int startX = atoi(tokens[2].c_str());
		int startY = atoi(tokens[3].c_str());

		camera_->SetTarget(id);
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

void Robot::SetUpdate(int x, int y, int heading, int pan, int battery, int status)
{
	loc->setX(x);
	loc->setY(y);
	if (heading == 0)
		robotHeading_ = NORTH;
	else if (heading == 1)
		robotHeading_ = EAST;
	else if (heading == 2)
		robotHeading_ = SOUTH;
	else if (heading == 3)
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

void Robot::Update()
{
	//anything that needs to be done continously (i.e. if we're continuously spinning the camera) can go here
	
	if (camera_->GetTargetVisible())
		centerCameraOnTarget();
	else 
		ExecuteCommand("pan 359");
}

void Robot::centerCameraOnTarget()
{
	string cmd = "pan ";
	int width = camera_->GetImageWidth();
	bool cam = camera_->GetDLinkCam();
	
	vector<TrackingObject*> visObjs = camera_->GetVisibleObjects();
	vector<TrackingObject*>::iterator voIter;
	for(voIter = visObjs.begin(); voIter != visObjs.end(); voIter++)
	{
		if(camera_->GetTargetID() == (*voIter)->GetID())
			break;
	}
	int d = (*voIter)->CenterDistanceToDegrees(width, cam);

	stringstream oss;
	oss << "pan " << d;
	cmd = oss.str();
	ExecuteCommand(cmd);
}

string Robot::newCmd()
{
	string cmd;
	if(!(robPath->getStart()))
	{
		return cmd;
	}
	else if(camera_->GetTargetVisible())
	{
		if(cameraDirection_ <= 45 || cameraDirection_ > 315)
		{
			cmd = "forward";
			setRobotMoving(true);
		}
		else if(cameraDirection_ <= 135 && cameraDirection_ > 45)
			cmd = "left";
		else if(cameraDirection_ <= 225 && cameraDirection_ > 135)
			cmd = "turnaround";
		else if(cameraDirection_ <= 315 && cameraDirection_ > 225)
			cmd = "right";
	}
	else if(loc->getX() < robPath->getStart()->getX())
	{
		switch(robotHeading_)
		{
		case NORTH:
			cmd = "right";
			printf("Turning right\n");
			robotHeading_ = EAST;
			break;
		case EAST:
			cmd = "forward";
			printf("Moving forward\n");
			//robPath->advPath();
			setRobotMoving(true);
			updateLocation();
			break;
		case SOUTH:
			cmd = "left";
			printf("Turning left\n");
			robotHeading_ = EAST;
			break;
		case WEST:
			cmd = "turnaround";
			printf("Turning around\n");
			robotHeading_ = EAST;
			break;
		}
	}
	else if(loc->getX() > robPath->getStart()->getX())
	{
		switch(robotHeading_)
		{
		case NORTH:
			cmd = "left";
			printf("Turning left\n");
			robotHeading_ = WEST;
			break;
		case EAST:
			cmd = "turnaround";
			printf("Turning around\n");
			robotHeading_ = WEST;
			break;
		case SOUTH:
			cmd = "right";
			printf("Turning right\n");
			robotHeading_ = WEST;
			break;
		case WEST:
			cmd = "forward";
			printf("Moving forward\n");
			//robPath->advPath();
			setRobotMoving(true);
			updateLocation();
			break;
		}
	}
	else if(loc->getY() < robPath->getStart()->getY())
	{
		switch(robotHeading_)
		{
		case NORTH:
			cmd = "forward";
			printf("Moving forward\n");
			//robPath->advPath();
			setRobotMoving(true);
			updateLocation();
			break;
		case EAST:
			cmd = "left";
			printf("Turning left\n");
			robotHeading_ = NORTH;
			break;
		case SOUTH:
			cmd = "turnaround";
			printf("Turning around\n");
			robotHeading_ = NORTH;
			break;
		case WEST:
			cmd = "right";
			printf("Turning right\n");
			robotHeading_ = NORTH;
			break;
		}
	}
	else if(loc->getY() > robPath->getStart()->getY())
	{
		switch(robotHeading_)
		{
		case NORTH:
			cmd = "turnaround";
			printf("Turning around\n");
			robotHeading_ = SOUTH;
			break;
		case EAST:
			cmd = "right";
			printf("Turning right\n");
			robotHeading_ = SOUTH;
			break;
		case SOUTH:
			cmd = "forward";
			printf("Moving forward\n");
			//robPath->advPath();
			setRobotMoving(true);
			updateLocation();
			break;
		case WEST:
			cmd = "left";
			printf("Turning left\n");
			robotHeading_ = SOUTH;
			break;
		}
	}

	return cmd;
}

void Robot::setDestination(GridLoc* newD)
{
	/*delete dest;*/
	dest = newD;
}

void Robot::setNextLoc(GridLoc* newNextLoc)
{
	//delete nextLoc;
	nextLoc = newNextLoc;
}

void Robot::setPath(Path* newPath)
{
	/*if(robPath)
		delete robPath;*/
	robPath = newPath;
	/*if(robPath)
		setNextLoc(robPath->advPath());*/
}

void Robot::updateLocation()
{
	//delete loc;
	//loc = nextLoc;
	//setNextLoc(robPath->advPath());
	loc = robPath->advPath();
}