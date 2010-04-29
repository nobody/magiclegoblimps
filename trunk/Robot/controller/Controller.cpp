#include "Controller.h"

//static member variables must be redeclared in source
vector<Robot*> Controller::robots_;
SOCKET Controller::connectSocket_;
char* Controller::port_;
int Controller::xMax;
int Controller::yMax;
bool Controller::connected_;
vector<GridLoc*> Controller::permIllegalLocs;
float Controller::timer_;
time_t Controller::lastTime_;
bool Controller::running_;
HANDLE Controller::robotSemaphore_;

Controller::Controller(int xDim, int yDim, string routerIP)
{
	xMax = xDim;
	yMax = yDim;

	Camera::SetRouterIP(routerIP);

	timer_= 0;
	connectSocket_ = NULL;
	port_ = "9999";

	robotSemaphore_ = CreateSemaphore(NULL, 1, 1, NULL);

	connected_ = false;
	running_ = true;

	_beginthread(UpdateThread, 0, NULL);
}

bool Controller::Connect(string ip)
{
	WSADATA wsaData;
	connectSocket_ = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int iResult = 0;
	int recvBufLen = BUFFER_LENGTH;

	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) 
	{
		printf("WSAStartup failed: %d\n", iResult);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(ip.c_str(), port_, &hints, &result);
	if (iResult != 0) 
	{
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return false;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		connectSocket_ = socket(ptr->ai_family, ptr->ai_socktype, 
			ptr->ai_protocol);
		if (connectSocket_ == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return false;
		}

		iResult = connect(connectSocket_, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			closesocket(connectSocket_);
			connectSocket_ = INVALID_SOCKET;
			connected_ = false;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (connectSocket_ == INVALID_SOCKET) 
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return false;
	}

	cout << "Connected to server." << endl;

	byteArray sendArray;
	robotInit* init;

	if (robots_.empty())
	{
		init = new robotInit;
		init->RID = -1;
		init->VideoURL = NULL;

		write_data(P_ROBOT_INIT, init, 1, &sendArray);

		delete init;
	}
	else
	{
		WaitForSingleObject(robotSemaphore_, INFINITE);

		init = new robotInit[robots_.size()];

		vector<Robot*>::iterator it;
		int i = 0;

		for (it = robots_.begin(); it != robots_.end(); it++)
		{
			init[i].RID = (*it)->GetID();
			if ((*it)->GetCamera()->GetDLinkCam())
				init[i].cameraType = P_DLINK;
			else
				init[i].cameraType = P_CISCO;
			init[i].VideoURL = new string((*it)->GetCamera()->GetExtURL());
			init[i].x = (*it)->getLocation()->getX();
			init[i].y = (*it)->getLocation()->getY();

			i++;
		}

		ReleaseSemaphore(robotSemaphore_, 1, NULL);

		write_data(P_ROBOT_INIT, init, (short)robots_.size(), &sendArray);

		delete[] init;
	}

	iResult = send(connectSocket_, sendArray.array, sendArray.size, 0);
	if (iResult == SOCKET_ERROR) 
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(connectSocket_);
		WSACleanup();
		connected_ = false;
	}

	_beginthread(ClientThread, 0, NULL);

	connected_ = true;

	return true;
}

void Controller::Disconnect()
{
	if (connectSocket_ != NULL)
	{
		shutdown(connectSocket_, SD_BOTH);
		closesocket(connectSocket_);
		WSACleanup();
		connected_ = false;
	}

	if (robots_.size() > 0)
	{
		vector<Robot*>::iterator it;

		for (it = robots_.begin(); it != robots_.end(); it++)
		{
			(*it)->StopRunning();
			delete (*it);
		}

		robots_.clear();
	}

	if (Camera::GetTrackableObjects()->size() > 0)
	{
		Vector_ts<TrackingObject*>::iterator it;
		Vector_ts<TrackingObject*>* objects = Camera::GetTrackableObjects();

		for (it = objects->begin(); it != objects->end(); it++)
			delete (*it);

		objects->clear();
	}

	CloseHandle(robotSemaphore_);
}

void Controller::ClientThread(void* params)
{
	int iResult = 0;
	char recvBuf[BUFFER_LENGTH];
	int recvBufLen = BUFFER_LENGTH;
	while (true)
	{
		iResult = recv(connectSocket_, recvBuf, recvBufLen, 0);
		if (iResult > 0)
		{
			readReturn* data = new readReturn;

			read_data(recvBuf, data);

			int type = data->type;

			if (type == P_OBJECT)
			{
				object* obj = (object*)data->array;

				for (int i = 0; i < data->size; i++)
				{
					//remove output after testing
					cout << "Adding Object " << obj[i].OID << 
						" : " << obj[i].name << endl;

					CvBox2D box = TrackingObject::ArrayToBox(obj[i].box);
					CvHistogram* hist = 
						TrackingObject::ArrayToHistogram(obj[i].color);
					CvScalar color = CV_RGB(0, 0, 0);

					TrackingObject* newObj = 
						new TrackingObject(hist, box, color);

					newObj->SetID(obj[i].OID);

					Vector_ts<TrackingObject*>* objects = 
						Camera::GetTrackableObjects();

					objects->lock();
					objects->push_back(newObj);
					objects->unlock();
				}
			}
			else if (type == P_ASSIGNMENT)
			{
				assignment* assign = (assignment*)data->array;

				for (int i = 0; i < data->size; i++)
				{
					//remove output after testing
					cout << "Assigning Robot " << assign[i].RID << 
						" to Object " << assign[i].OID << endl;

					string cmd;
					stringstream oss;
					oss << "target " << assign[i].OID << " " << assign[i].x << 
						" " << assign[i].y;
					cmd = oss.str();

					Robot* robot = GetRobot(assign[i].RID);

					WaitForSingleObject(robot->GetSemaphore(), INFINITE);
					robot->ExecuteCommand(cmd);
					ReleaseSemaphore(robot->GetSemaphore(), 1, NULL);
				}

				delete[] assign;
			}
			else if (type == P_COMMAND)
			{
				command* comm = (command*)data->array;

				for (int i = 0; i < data->size; i++)
				{
					//remove output after testing
					cout << "Command Robot " << comm[i].RID << 
						" to " << comm[i].cmd << " with arg " << comm[i].arg <<
						endl;

					Command(comm[i].RID, comm[i].cmd, comm[i].arg);
				}

				delete[] comm;
			}

			delete data;
		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
			_endthread();
		}
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} 
}

bool Controller::Command(int id, int command, int arg)
{
	if (command == P_CMD_FWD)
	{
		GetRobot(id)->ExecuteCommand("forward");
	}
	else if(command == P_CMD_LFT)
	{
		GetRobot(id)->ExecuteCommand("left");
	}
	else if(command == P_CMD_RGHT)
	{
		GetRobot(id)->ExecuteCommand("right");
	}
	else if (command == P_CMD_CAMROT)
	{
		string cmd = "";
		stringstream oss;
		oss << "pan " << -arg;
		cmd = oss.str();
		GetRobot(id)->ExecuteCommand(cmd);
	}
	else if (command == P_CMD_DEL_OBJ)
	{
		Camera::RemoveTrackingObject(arg);
	}
	else if (command == P_CMD_SHUTDOWN)
	{
		Disconnect();
	}

	return true;
}

void Controller::AddRobot(Robot* robot)
{
	RobotParams* rp = new RobotParams;
	rp->robot = robot;
	robot->StartRunning();
	robot->setSearchLoc(xMax / 2, yMax / 2);
	_beginthread(RobotThread, 0, rp);
	WaitForSingleObject(robotSemaphore_, INFINITE);
	robots_.push_back(robot);
	ReleaseSemaphore(robotSemaphore_, 1, NULL);
	SendRobot(robot->GetID());
}

Robot* Controller::GetRobot(int id)
{
	vector<Robot*>::iterator it;
	for (it = robots_.begin(); it != robots_.end(); it++)
	{
		if ((*it)->GetID() == id)
			return (*it);
	}

	return NULL;
}

void Controller::RemoveRobot(int id)
{
	WaitForSingleObject(robotSemaphore_, INFINITE);
	vector<Robot*>::iterator it;
	for (it = robots_.begin(); it != robots_.end(); it++)
	{
		if ((*it)->GetID() == id)
		{
			(*it)->StopRunning();
			delete (*it);
			robots_.erase(it);
			break;
		}
	}
	ReleaseSemaphore(robotSemaphore_, 1, NULL);
}

void Controller::SendRobot(int id)
{
	if (!connected_)
		return;

	WaitForSingleObject(robotSemaphore_, INFINITE);

	byteArray sendArray;
	robotInit* init;
	Robot* robot = GetRobot(id);

	init = new robotInit;

	init->RID = robot->GetID();
	if (robot->GetCamera()->GetDLinkCam())
		init->cameraType = P_DLINK;
	else
		init->cameraType = P_CISCO;
	init->VideoURL = new string(robot->GetCamera()->GetExtURL());
	init->x = robot->getLocation()->getX();
	init->y = robot->getLocation()->getY();

	ReleaseSemaphore(robotSemaphore_, 1, NULL);

	write_data(P_ROBOT_INIT, init, 1, &sendArray);

	int iResult = 0;

	iResult = send(connectSocket_, sendArray.array, sendArray.size, 0);
	if (iResult == SOCKET_ERROR) 
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(connectSocket_);
		WSACleanup();
		connected_ = false;
	}

	delete init;
}

void Controller::SendObject(int id)
{
	if (!connected_)
		return;

	byteArray sendArray;
	object* objects = new object;
	TrackingObject* obj = Camera::GetTrackingObject(id);

	string n = "";
	stringstream oss;
	oss << "Animal" << obj->GetID();
	n = oss.str();
	string* name = new string (n);

	char* barr;
	int bsize;

	objects->OID = obj->GetID();
	objects->name = name;

	barr = TrackingObject::BoxToArray(obj->GetOriginalBox(), &bsize);
	objects->box = barr;
	objects->box_size = bsize;

	char* harr;
	int hsize;

	harr = TrackingObject::HistogramToArray(obj->GetHistogram(), &hsize);
	objects->color = harr;
	objects->color_size = hsize;

	write_data(P_OBJECT, objects, 1, &sendArray);

	int iResult = 0;

	iResult = send(connectSocket_, sendArray.array, sendArray.size, 0);

	if (iResult == SOCKET_ERROR) 
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(connectSocket_);
		WSACleanup();
		connected_ = false;
	}

	delete objects;
}

void Controller::UpdateThread(void* params)
{
	while (running_)
	{
		time_t seconds = time(NULL);
		float interval = (float)seconds - lastTime_;
		lastTime_ = seconds;

		timer_ += interval;

		if (timer_ > SEND_INTERVAL)
		{
			vector<Robot*>::iterator it;

			if (connected_ && (robots_.size() > 0))
			{
				byteArray sendArray;

				WaitForSingleObject(robotSemaphore_, INFINITE);

				robotUpdate* update = new robotUpdate[robots_.size()];

				int i = 0;

				for (it = robots_.begin(); it != robots_.end(); it++)
				{
					WaitForSingleObject((*it)->GetSemaphore(), INFINITE);

					update[i].RID = (*it)->GetID();
					update[i].dir = (*it)->getHeading();
					update[i].x = (*it)->getLocation()->getX();
					update[i].y = (*it)->getLocation()->getY();
					update[i].listSize = 
						(int)(*it)->GetCamera()->GetVisibleObjects().size();

					if (update[i].listSize > 0)
					{
						int* objects = new int[update[i].listSize];
						for (int j = 0; j < update[i].listSize; j++)
						{
							objects[j] = 
								(*it)->GetCamera()->GetVisibleObjects()[j]->GetID();
						}
						update[i].objects = objects;

						float* qualities = new float[update[i].listSize];
						for (int j = 0; j < update[i].listSize; j++)
						{
							qualities[j] = 
								(*it)->GetCamera()->GetVisibleObjects()[j]->GetQuality(
								(*it)->GetCamera()->GetImageWidth());
						}
						update[i].qualities = qualities;

						int* xs = new int[update[i].listSize];
						int* ys = new int[update[i].listSize];

						if ((*it)->GetCamConnected() && (*it)->GetNXTConnected())
						{
							for (int j = 0; j < update[i].listSize; j++)
							{
								xs[j] = (*it)->GetObjectLocation(objects[j])->getX();
								ys[j] = (*it)->GetObjectLocation(objects[j])->getY();
							}
						}
						else
						{
							for (int j = 0; j < update[i].listSize; j++)
							{
								xs[j] = 0;
								ys[j] = 0;
							}
						}

						update[i].xs = xs;
						update[i].ys = ys;
					}

					ReleaseSemaphore((*it)->GetSemaphore(), 1, NULL);

					i++;
				}

				ReleaseSemaphore(robotSemaphore_, 1, NULL);

				write_data(P_ROBOT_UPDATE, update, (short)robots_.size(), &sendArray);

				delete[] update;

				int iResult = 0;

				iResult = send(connectSocket_, sendArray.array, sendArray.size, 0);
				if (iResult == SOCKET_ERROR) 
				{
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(connectSocket_);
					WSACleanup();
					connected_ = false;
				}
			}

			timer_ = 0;
		}
	}
}

void Controller::RobotThread(void* params)
{
	RobotParams* rp = (RobotParams*)params;
	Robot* robot = rp->robot;

	robot->Connect();

	while (robot->GetRunning())
	{
		robot->Update();

		if (robot->GetNXTConnected())
		{
			WaitForSingleObject(robot->GetSemaphore(), INFINITE);

			try
			{	
				string update = robot->GetNXT()->ReadMessage();

				vector<string> tokens;
				tokenize(update, tokens, " ");

				if (tokens[0].compare("pan") == 0)
				{
					robot->SetUpdatePan(atoi(tokens[1].c_str()));
					robot->SetCameraPanning(false);
				}
				else
				{
					robot->SetUpdateMovement(atoi(tokens[0].c_str()),
						atoi(tokens[1].c_str()),
						atoi(tokens[2].c_str()),
						atoi(tokens[3].c_str()),
						atoi(tokens[4].c_str()));

					cout << "Robot: " << robot->getID();
					cout << " Loc: " << robot->getLocation()->getX() << ", " << robot->getLocation()->getY(); 
					cout << " Status: " << robot->getStatus();
					cout << " Heading: " << robot->getHeading();
					cout << " Batt: " << robot->getBatt();
					cout << " Pan: " << robot->getCamDir() << endl;
				}

				robot->setRobotMoving(false);

				if(!(robot->getStatus() & (INTERSECTION | IDLE)))
				{}
				else if(robot->GetCamera()->GetTargetVisible())
					robot->GetNXT()->SendMessage(newCmd(robot));
				else if(*(robot->getLocation()) == *(robot->getDestination())
					|| !robot->getHasDest())
				{
					if (robot->GetCamera()->GetTargetID() != -1)
						robot->GetNXT()->SendMessage(newCmd(robot));
				}
				else if(!robot->getHasPath())
				{
					robot->setPath(genPath(*robot));
					robot->GetNXT()->SendMessage(newCmd(robot));
				}
				else if(robot->getLocation()->calcDist(*robot->getPath()->getStart()) != 1)
				{
					robot->setPath(genPath(*robot));
					robot->GetNXT()->SendMessage(newCmd(robot));
				}
				else
				{
					robot->GetNXT()->SendMessage(newCmd(robot));
				}
			}
			catch (Nxt_exception& e)
			{
				//keep the empty mailbox exceptions quiet
				/*
				cout << e.what() << endl;
				cout << e.error_code() << endl;
				cout << e.error_type() << endl;
				cout << e.who() << endl;
				*/
			}

			ReleaseSemaphore(robot->GetSemaphore(), 1, NULL);
		}
	}

	robot->Disconnect();
}

Path* Controller::genPath(Robot& robot)
{
	priority_queue<Path*, vector<Path*>, Path> pathHeap;
	vector<GridLoc*> illMoves = getIllMoves();
	vector<GridLoc*> moves = getValidMoves(*robot.getLocation(),
		illMoves);

	/*if(robot.getLocation() == robot.getDestination())
	{
	Path* newPth = new Path();*/


	//Make sure the destination is not in the list of illegal
	//locations
	vector<GridLoc*>::iterator glIter;
	for(glIter = illMoves.begin(); glIter != illMoves.end(); glIter++)
	{
		if((*glIter) == robot.getDestination())
			illMoves.erase(glIter);
	}

	Path* pth;
	vector<GridLoc*>::iterator mvIter;
	for(mvIter = moves.begin(); mvIter != moves.end(); mvIter++)
	{
		pth = new Path();
		pth->extend((*mvIter));
		pth->calcMetric(*robot.getLocation(),
			*robot.getDestination());
		pathHeap.push(pth);
		illMoves.push_back(*mvIter);
	}

	if(pathHeap.size() != 0)
	{
		Path* best = pathHeap.top();
		pathHeap.pop();
		while(*best->getEnd() != *robot.getDestination())
		{
			moves = getValidMoves(*best->getEnd(), illMoves);
			for(mvIter= moves.begin(); mvIter != moves.end(); mvIter++)
			{
				Path* newPath = best->copy();
				newPath->extend(*mvIter);
				newPath->calcMetric(*robot.getLocation(),
					*robot.getDestination());
				pathHeap.push(newPath);
				illMoves.push_back(*mvIter);
			}

			if(pathHeap.size() != 0)
			{
				best = pathHeap.top();
				pathHeap.pop();
			}
			else
			{
				return best;
			}
		}
		return best;
	}
	else 
		return NULL;
}

vector<GridLoc*> Controller::getValidMoves(GridLoc loc, 
	vector<GridLoc*> illMoves)
{
	vector<GridLoc*> moves;

	int xLoc = loc.getX();
	int yLoc = loc.getY();

	if(xLoc > 0 && xLoc < xMax)
	{
		GridLoc* xplus = new GridLoc(xLoc+1, yLoc);
		GridLoc* xminus = new GridLoc(xLoc-1, yLoc);
		moves.push_back(xplus);
		moves.push_back(xminus);
	}
	else if(xLoc == 0)
	{
		GridLoc* xplus = new GridLoc(xLoc+1, yLoc);
		moves.push_back(xplus);
	}
	else if(xLoc == xMax)
	{
		GridLoc* xminus = new GridLoc(xLoc-1, yLoc);
		moves.push_back(xminus);
	}

	if(yLoc > 0 && yLoc < yMax)
	{
		GridLoc* yplus = new GridLoc(xLoc, yLoc+1);
		GridLoc* yminus = new GridLoc(xLoc, yLoc-1);
		moves.push_back(yplus);
		moves.push_back(yminus);
	}
	else if(yLoc == 0)
	{
		GridLoc* yplus = new GridLoc(xLoc, yLoc+1);
		moves.push_back(yplus);
	}
	else if(yLoc == yMax)
	{
		GridLoc* yminus = new GridLoc(xLoc, yLoc-1);
		moves.push_back(yminus);
	}

	vector<GridLoc*>::iterator imIter;
	for(imIter = illMoves.begin(); imIter != illMoves.end(); imIter++)
	{
		vector<GridLoc*>::iterator mvIter;
		for(mvIter = moves.begin(); mvIter != moves.end(); mvIter++)
		{
			if(*mvIter == *imIter)
			{
				moves.erase(mvIter);
				break;
			}
		}
	}

	return moves;
}

vector<GridLoc*> Controller::getIllMoves()
{
	vector<GridLoc*> illMoves;

	vector<Robot*>::iterator it;
	for(it = robots_.begin(); it != robots_.end(); it++)
	{
		if((*it)->getRobotMoving())
			illMoves.push_back(new GridLoc((*it)->getPath()->getStart()->getX(),
			(*it)->getPath()->getStart()->getY()));
		illMoves.push_back(new GridLoc((*it)->getLocation()->getX(),
			(*it)->getLocation()->getY()));
	}
	return illMoves;
}

void Controller::SearchObject(Robot* robot)
{
	if(robot->getSearchLoc()->getY() == -1)
	{
		SpiralSearch(robot, new GridLoc(yMax / 2, xMax / 2));
	}
	else if (robot->getLocation()->getX() == robot->getSearchLoc()->getX() &&
		robot->getLocation()->getY() == robot->getSearchLoc()->getY())
	{
		SpiralSearch(robot, robot->getLocation());
	}
	else
	{
		robot->setDestination(robot->getSearchLoc());
		robot->setPath(genPath(*robot));
	}
}

void Controller::SpiralSearch(Robot* robot, GridLoc* loc)
{
	robot->setDestination(loc);
	robot->setPath(genPath(*robot));

	int x = loc->getX();
	int y = loc->getY();
	int xdir = 1;
	int ydir = 1;

	if (xMax >= yMax) //heading east
	{  
		xdir = 1;
		ydir = 1;
	}
	else //heading south
	{
		xdir = 1;
		ydir = -1;
	}

	int dir = 1; //1 is x, -1 is y

	int countSpirals = 1;

	while(countSpirals < xMax/2 && countSpirals < yMax/2)
	{
		for (int countTurns = 0; countTurns < 2; countTurns++) 
		{
			if (dir == 1) 
			{
				x += xdir*countSpirals;
				xdir *= -1;
				loc->setX(x);
				Path* p = robot->getPath();
				p->extend(loc);
			}
			else {
				y += ydir*countSpirals;
				ydir *= -1;
				loc->setY(y);
				Path* p = robot->getPath();
				p->extend(loc);
			}			
			dir *= -1; //change heading
		}		
		countSpirals++;
	}
}

string Controller::newCmd(Robot* rob)
{
	string cmd;
	if(rob->GetCamera()->GetTargetVisible())
	{
		rob->setHasDest(false);
		if(rob->getCamDir() <= 45 || rob->getCamDir() > 315)
		{
			cmd = "forward";
			rob->setRobotMoving(true);
			cout << "forward-tracking" << endl;
		}
		else if(rob->getCamDir() <= 135 && rob->getCamDir() > 45)
		{
			cmd = "left";
			cout << "left-turn-tracking" << endl;
			rob->ExecuteCommand("pan 90");
		}
		else if(rob->getCamDir() <= 225 && rob->getCamDir() > 135)
		{
			cmd = "turnaround";
			cout << "turnaround-tracking" << endl;
			rob->ExecuteCommand("pan 180");
		}
		else if(rob->getCamDir() <= 315 && rob->getCamDir() > 225)
		{
			cmd = "right";
			cout << "right-turn-tracking" << endl;
			rob->ExecuteCommand("pan -90");
		}
	}
	else if(!rob->getHasPath())
	{
		SearchObject(rob);
		return cmd;
	}
	else if(rob->getLocation()->getX() < rob->getPath()->getStart()->getX())
	{
		switch(rob->getHeading())
		{
		case NORTH:
			cmd = "right";
			printf("Turning right\n");
			rob->setHeading(EAST);
			break;
		case EAST:
			cmd = "forward";
			printf("Moving forward\n");
			rob->setRobotMoving(true);
			rob->updateLocation();
			break;
		case SOUTH:
			cmd = "left";
			printf("Turning left\n");
			rob->setHeading(EAST);
			break;
		case WEST:
			cmd = "turnaround";
			printf("Turning around\n");
			rob->setHeading(EAST);
			break;
		}
	}
	else if(rob->getLocation()->getX() > rob->getPath()->getStart()->getX())
	{
		switch(rob->getHeading())
		{
		case NORTH:
			cmd = "left";
			printf("Turning left\n");
			rob->setHeading(WEST);
			break;
		case EAST:
			cmd = "turnaround";
			printf("turnaround\n");
			rob->setHeading(WEST);
			break;
		case SOUTH:
			cmd = "right";
			printf("Turning right\n");
			rob->setHeading(WEST);
			break;
		case WEST:
			cmd = "forward";
			printf("moving forward\n");
			rob->setRobotMoving(true);
			rob->updateLocation();
			break;
		}
	}
	else if(rob->getLocation()->getY() < rob->getPath()->getStart()->getY())
	{
		switch(rob->getHeading())
		{
		case NORTH:
			cmd = "forward";
			printf("moving forward\n");
			rob->setRobotMoving(true);
			rob->updateLocation();
			break;
		case EAST:
			cmd = "left";
			printf("turn left\n");
			rob->setHeading(NORTH);
			break;
		case SOUTH:
			cmd = "turnaround";
			printf("turnaround\n");
			rob->setHeading(NORTH);
			break;
		case WEST:
			cmd = "right";
			printf("turn right\n");
			rob->setHeading(NORTH);
			break;
		}
	}
	else if(rob->getLocation()->getY() > rob->getPath()->getStart()->getY())
	{
		switch(rob->getHeading())
		{
		case NORTH:
			cmd = "turnaround";
			printf("turnaround\n");
			rob->setHeading(SOUTH);// = EAST;
			break;
		case EAST:
			cmd = "right";
			printf("turn right\n");
			//robPath->advPath();
			rob->setHeading(SOUTH);
			//rob->setRobotMoving(true);
			//rob->updateLocation();
			break;
		case SOUTH:
			cmd = "forward";
			printf("moving forward\n");
			rob->setRobotMoving(true);
			rob->updateLocation();
			//rob->setHeading(WEST);
			//robotHeading_ = EAST;
			break;
		case WEST:
			cmd = "left";
			printf("turn left\n");
			rob->setHeading(SOUTH);
			//rob->setRobotMoving(true);
			//rob->updateLocation();
			break;
		}
	}

	return cmd;
}

