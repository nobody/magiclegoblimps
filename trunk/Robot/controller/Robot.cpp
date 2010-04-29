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

	hasPath = false;
	hasDest = false;

	cameraPanning_ = false;

	batteryLevel_ = 0;
	status_ = 0;

	loc = new GridLoc();
	dest = new GridLoc();
	searchLoc = new GridLoc(0, -1);

	robotHeading_ = NORTH;
	cameraDirection_ = 0;

	panTime_ = 1;

	updateSemaphore_ = CreateSemaphore(NULL, 1, 1, NULL);
	
	running_ = false;
}

Robot::~Robot()
{
	delete nxt_;
	delete camera_;

	CloseHandle(updateSemaphore_);
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
	vector<TrackingObject*> visobjects = camera_->GetVisibleObjects();

	for (it = visobjects.begin(); it != visobjects.end(); it++)
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
	
	//catch NXT dead exception

	WaitForSingleObject(updateSemaphore_, INFINITE);
	
	if (tokens[0].compare("target") == 0)
	{
		int id = atoi(tokens[1].c_str());

		if (tokens.size() > 2)
		{
			int startX = atoi(tokens[2].c_str());
			int startY = atoi(tokens[3].c_str());

			searchLoc->setX(startX);
			searchLoc->setY(startY);
		}

		camera_->SetTarget(id);
	}
	else if (tokens[0].compare("pan") == 0 || tokens[0].compare("pid") == 0)
	{
		if (!cameraPanning_)
		{
			cameraPanning_ = true;
			nxt_->SendMessage(command, 3);
		}
	}
	else
	{
		//command
		//=======
		//pan x
		//pid x
		//stop
		//left
		//right
		//turnaround
		//calibrate
		//forward
		//forward x
		//coordinates x y

		nxt_->SendMessage(command);
	}

	ReleaseSemaphore(updateSemaphore_, 1, NULL);
}

void Robot::SetUpdateMovement(int x, int y, int heading, int battery, int status)
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
	
	batteryLevel_ = battery;
	status_ = status;
}

void Robot::SetUpdatePan(int pan)
{
	cameraDirection_ = pan;
}

void Robot::Update()
{
	time_t seconds = time(NULL);
	float interval = (float)seconds - lastTime_;
	lastTime_ = seconds;

	timer_ += interval;

	camera_->Update();

	if (camera_->GetLocalDisplay())
		camera_->DisplayFrame();

	if (nxtConnected_ && camConnected_ && (camera_->GetTargetID() != -1))
	{
		if (camera_->GetTargetVisible())
			centerCameraOnTarget();
		else 
			ExecuteCommand("pan 1");
	}
}

void Robot::centerCameraOnTarget()
{
	string cmd = "pan ";
	int width = camera_->GetImageWidth();
	bool cam = camera_->GetDLinkCam();
	int d = 0;

	vector<TrackingObject*> visObjs = camera_->GetVisibleObjects();
	vector<TrackingObject*>::iterator voIter;
	for(voIter = visObjs.begin(); voIter != visObjs.end(); voIter++)
	{
		if (voIter == visObjs.end())
			break;

		if(camera_->GetTargetID() == (*voIter)->GetID())
			d = (*voIter)->CenterDistanceToDegrees(width, cam);
	}

	if (d > 0)
		ExecuteCommand("pan 2");
	else if (d < 0)
		ExecuteCommand("pan -2");

	/*
	stringstream oss;
	oss << "pan " << d;
	cmd = oss.str();
	ExecuteCommand(cmd);
	*/
}

string Robot::newCmd()
{
	string cmd;
	
	if(camera_->GetTargetVisible())
	{
		hasDest = false;
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
	else if(!hasPath)
	{

		return cmd;
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
	hasDest = true;
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
	hasPath = true;
	/*if(robPath)
		setNextLoc(robPath->advPath());*/
}

void Robot::updateLocation()
{
	//delete loc;
	//loc = nextLoc;
	//setNextLoc(robPath->advPath());
	loc = robPath->advPath();
	if(robPath->getSize() == 0)
	{
		hasPath = false;
		hasDest = false;
	}
}

void Robot::setSearchLoc(int x, int y)
{
	searchLoc->setX(x);
	searchLoc->setY(y);
}