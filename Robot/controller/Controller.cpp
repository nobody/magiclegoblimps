#include "Controller.h"

//static member variables must be redeclared in source
vector<Robot*> Controller::robots_;
SOCKET Controller::connectSocket_;

Controller::Controller()
{
	timer_ = 0;
	connectSocket_ = NULL;
	serverSocket_ = NULL;
	port_ = "9999";
	
	lastObjectSize_ = 0;
	lastRobotSize_ = 0;

	connected_ = false;
}

Controller::Controller(int xDim, int yDim)
{
	xMax = xDim;
	yMax = yDim;

	timer_= 0;
	connectSocket_ = NULL;
	serverSocket_ = NULL;
	port_ = "9999";

	lastObjectSize_ = 0;
	lastRobotSize_ = 0;

	connected_ = false;
}

bool Controller::ConnectToServer(string ip)
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

	//send robot init stuff
	byteArray sendArray;
	robotInit* init;

	if (robots_.empty())
	{
		init = new robotInit[1];
		init[0].RID = -1;
		init[0].VideoURL = NULL;

		write_data(P_ROBOT_INIT, init, 1, &sendArray);
	}
	else
	{
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
			init[i].VideoURL = new string((*it)->GetCamera()->GetVideoURL());
			init[i].x = (*it)->getLocation()->getX();
			init[i].y = (*it)->getLocation()->getY();

			i++;
		}

		write_data(P_ROBOT_INIT, init, (short)robots_.size(), &sendArray);

		//delete[] init;
	}

	iResult = send(connectSocket_, sendArray.array, sendArray.size, 0);
    if (iResult == SOCKET_ERROR) 
	{
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(connectSocket_);
        WSACleanup();
		connected_ = false;
    }

	delete[] init;

	_beginthread(ClientThread, 0, NULL);

	connected_ = true;

	return true;
}

//creates a listening server socket for testing with self
bool Controller::Serve()
{
	WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, hints;
    int iResult = 0;
    
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
	{
        printf("WSAStartup failed: %d\n", iResult);
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, port_, &hints, &result);
    if (iResult != 0) 
	{
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return false;
    }

    listenSocket = socket(result->ai_family, result->ai_socktype, 
		result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) 
	{
        printf("socket failed: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) 
	{
        printf("bind failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result);

    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) 
	{
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    serverSocket_ = accept(listenSocket, NULL, NULL);
    if (serverSocket_ == INVALID_SOCKET) 
	{
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    closesocket(listenSocket);

	cout << "Client connected." << endl;

    return true;
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
					cout << "Adding Object " << obj[i].OID << 
						" : " << obj[i].name << endl;

					/*
					CvBox2D box = TrackingObject::ArrayToBox(obj[i].box);
					CvHistogram* hist = 
						TrackingObject::ArrayToHistogram(obj[i].color);
					CvScalar color = CV_RGB(0, 0, 0);

					TrackingObject* newObj = 
						new TrackingObject(hist, box, color);

					newObj->SetID(obj[i].OID);

					Camera::GetTrackableObjects().push_back(newObj);

					delete hist;
					*/
				}
			}
			else if (type == P_ASSIGNMENT)
			{
				assignment* assign = (assignment*)data->array;

				for (int i = 0; i < data->size; i++)
				{
					cout << "Assigning Robot " << assign[i].RID << 
						" to Object " << assign[i].OID << endl;

					//can't call non-static from in a static
					//need to sort this out
					/*
					GetRobot(assign[i].RID)->ExecuteCommand("target " + 
						assign[i].OID + " " + assign[i].x + " " + assign[i].y);
					*/
				}

				delete[] assign;
			}
			else if (type == P_COMMAND)
			{
				command* comm = (command*)data->array;

				for (int i = 0; i < data->size; i++)
				{
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
	//example of command translation (not actual command)
	if (command == P_CMD_FWD)
	{
		//can't call non-static from in a static
		//GetRobot(id)->ExecuteCommand("forward " + arg);
	}
	else if(command == P_CMD_LFT)
	{
	}
	else if(command == P_CMD_RGHT)
	{
	}
	else if (command == P_CMD_CAMROT)
	{
	}
	else if (command == P_CMD_DEL_OBJ)
	{
		//delete an object from the trackableObjects
	}
	else if (command == P_CMD_SHUTDOWN)
	{
		//disconnect and then shutdown
	}

	return true;
}

//after creating a server and connecting a client, can be used to send
//test commands as if they were from the server
bool Controller::TestServer(int type)
{
	byteArray sendArray;

	if (type == P_OBJECT)
	{
	}
	else if (type == P_ASSIGNMENT)
	{
		//set test assignments here
		assignment* assign = new assignment[2];
		assign[0].OID = 1;
		assign[0].RID = 2;
		assign[1].OID = 3;
		assign[1].RID = 4;

		write_data(P_ASSIGNMENT, assign, 2, &sendArray);

		delete[] assign;
	}
	else if (type == P_COMMAND)
	{
	}

	int iResult = 0;

	iResult = send(serverSocket_, sendArray.array, sendArray.size, 0);
    if (iResult == SOCKET_ERROR) 
	{
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(serverSocket_);
        WSACleanup();
        return false;
    }

	return true;
}

void Controller::AddRobot(Robot* robot)
{
	robots_.push_back(robot);
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
	vector<Robot*>::iterator it;

	for (it = robots_.begin(); it != robots_.end(); it++)
	{
		if ((*it)->GetID() == id)
		{
			(*it)->Disconnect();
			delete (*it);
			robots_.erase(it);
			break;
		}
	}
}

vector<Robot*> Controller::GetRobotVector()
{
	return robots_;
}

void Controller::Disconnect()
{
	if (robots_.size() > 0)
	{
		vector<Robot*>::iterator it;

		for (it = robots_.begin(); it != robots_.end(); it++)
		{
			(*it)->Disconnect();
			delete (*it);
		}

		robots_.clear();
	}

	//vectors incompatible error
	//needs fixing for cleaning up
	/*
	if (Camera::GetTrackableObjects().size() > 0)
	{
		vector<TrackingObject*>::iterator it;

		for (it = Camera::GetTrackableObjects().begin(); it != 
			Camera::GetTrackableObjects().end(); it++)
		{
			delete (*it);
		}

		Camera::GetTrackableObjects().clear();
	}
	*/
	
	if (connectSocket_ != NULL)
	{
		//socket might need shutdown
		closesocket(connectSocket_);
	    WSACleanup();
		connected_ = false;
	}

	if (serverSocket_ != NULL)
	{
		//socket might need shutdown
		closesocket(serverSocket_);
	    WSACleanup();
	}
}

void Controller::Update()
{
	time_t seconds = time(NULL);
	float interval = (float)seconds - lastTime_;
	lastTime_ = seconds;

	timer_ += interval;

	if (connected_ && (robots_.size() > 0))
	{
		if (lastRobotSize_ != robots_.size())
		{
			lastRobotSize_ = robots_.size();

			byteArray sendArray;
			robotInit* init;

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
				init[i].VideoURL = new string((*it)->GetCamera()->GetVideoURL());
				init[i].x = (*it)->getLocation()->getX();
				init[i].y = (*it)->getLocation()->getY();

				i++;
			}

			write_data(P_ROBOT_INIT, init, (short)robots_.size(), &sendArray);

			int iResult = 0;

			iResult = send(connectSocket_, sendArray.array, sendArray.size, 0);
			if (iResult == SOCKET_ERROR) 
			{
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(connectSocket_);
				WSACleanup();
				connected_ = false;
			}

			delete[] init;
		}
	}

	if (timer_ > POLL_INTERVAL)
	{
		vector<Robot*>::iterator it;

		for (it = robots_.begin(); it != robots_.end(); it++)
		{
			if ((*it)->GetNXTConnected())
			{
				try
				{
					string update = (*it)->GetNXT()->ReadMessage();

					vector<string> tokens;
					tokenize(update, tokens, " ");

					(*it)->SetUpdate(atoi(tokens[0].c_str()),
						atoi(tokens[1].c_str()),
						atoi(tokens[2].c_str()),
						atoi(tokens[3].c_str()),
						atoi(tokens[4].c_str()),
						atoi(tokens[5].c_str()));

					(*it)->setRobotMoving(false);

					if(!((*it)->getStatus() & (INTERSECTION | IDLE | STOP)))
					{continue;
					}
					else if((*it)->GetCamera()->GetTargetVisible())
						(*it)->GetNXT()->SendMessage((*it)->newCmd());
					else if((*it)->getLocation() == (*it)->getDestination())
					{continue;}
					else if(!(*it)->getHasPath())
					{
						(*it)->setPath(genPath(*(*it)));
						(*it)->GetNXT()->SendMessage((*it)->newCmd());
					}
					else if((*it)->getLocation()->calcDist(*(*it)->getPath()->getStart()) != 1)
					{
						(*it)->setPath(genPath(*(*it)));
						(*it)->GetNXT()->SendMessage((*it)->newCmd());
					}
					else
					{
						(*it)->GetNXT()->SendMessage((*it)->newCmd());
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
			}
		}

		if (connected_ && (robots_.size() > 0))
		{
			byteArray sendArray;

			robotUpdate* update = new robotUpdate[robots_.size()];

			int i = 0;

			for (it = robots_.begin(); it != robots_.end(); it++)
			{
				update[i].RID = (*it)->GetID();
				update[i].dir = (*it)->getHeading();
				update[i].x = (*it)->getLocation()->getX();
				update[i].y = (*it)->getLocation()->getY();
				update[i].listSize = 
					(int)(*it)->GetCamera()->GetVisibleObjects().size();

				cout << "LISTSIZE: " << update[i].listSize << endl;

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
					for (int j = 0; j < update[i].listSize; j++)
					{
						xs[j] = (*it)->GetObjectLocation(objects[j])->getX();
						ys[j] = (*it)->GetObjectLocation(objects[j])->getY();
					}
				}

				i++;
			}

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

	if (connected_ && (Camera::GetTrackableObjects().size() > 0))
	{
		if (lastObjectSize_ != Camera::GetTrackableObjects().size())
		{
			lastObjectSize_ = Camera::GetTrackableObjects().size();

			byteArray sendArray;

			object* objects = new object[lastObjectSize_];

			int i = 0;

			vector<TrackingObject*>::iterator it;

			for (it = Camera::GetTrackableObjects().begin(); 
				it != Camera::GetTrackableObjects().end(); it++)
			{
				string name = "Animal" + (*it)->GetID();

				char* arr;
				int size;

				objects[i].OID = (*it)->GetID();
				objects[i].name = &name;

				arr = TrackingObject::BoxToArray((*it)->GetOriginalBox(), &size);
				objects[i].box = arr;
				objects[i].box_size = size;

				arr = TrackingObject::HistogramToArray((*it)->GetHistogram(), &size);
				objects[i].color = arr;
				objects[i].color_size = size;

				delete arr;

				i++;
			}

			write_data(P_ROBOT_UPDATE, objects, (short)lastObjectSize_, &sendArray);

			delete[] objects;

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
	}

	vector<Robot*>::iterator it;

	for (it = robots_.begin(); it != robots_.end(); it++)
	{
		(*it)->Update();
	}
}

Path* Controller::genPath(Robot& robot)
{
	priority_queue<Path*, vector<Path*>, Path> pathHeap;
	vector<GridLoc*> illMoves = getIllMoves();
	vector<GridLoc*> moves = getValidMoves(*robot.getLocation(),
		illMoves);

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

void Controller::SearchObject(int robotID, int objID, GridLoc* lastKnownLoc)
{
	Robot* robot = GetRobot(robotID);
	Camera* camera = robot->GetCamera();
	
	camera->SetTarget(objID);
	
	//TODO: camera thread/spinning
	//while (!(camera->GetTargetVisible()))
	//{
	//	//TODO: SPIN CAMERA.
 //           //This needs to happen continuously while the robot is moving through
 //           //the spiral search. Maybe a separate thread?
	//}
	if(!lastKnownLoc)
	{
		//this doesn't build correctly (something to do with camera)
		//SpiralSearch(robot, camera, new GridLoc(yMax/2, xMax/2));
	}
	else if(!camera->GetTargetVisible())
	{
		robot->setDestination(lastKnownLoc);
		robot->setPath(genPath(*robot));
	}
	else
	{
		robot->setDestination(robot->GetObjectLocation(objID));
		robot->setPath(genPath(*robot));
	}

	//if (!(camera->GetTargetVisible()))
	//{
	//	//send to last known location
	//	robot->setDestination(lastKnownLoc);
	//	//genPath(robot);
	//	
	//	//while () {
	//	//	//wait until travel is complete
	//	//}
	//}
	//
	//if (!(camera->GetTargetVisible()))
	//{
	//	//SpiralSearch(robot, center); //TODO: need to set center coordinates for grid
	//}
	//else
	//{
	//	robot->GetObjectLocation(objID);
	//}
	
}

void Controller::SpiralSearch(Robot* robot, Camera& camera, GridLoc* loc)
{
	robot->setDestination(loc);
	//genPath(robot);
	
	int x = loc->getX();
	int y = loc->getY();
	
	int xdir = 1; 
	int ydir = 1;	
	int xory = 1; //1 is x, -1 is y
	
	int countSpirals = 1;
	int countSquares;
	
	while(!(camera.GetTargetVisible())) // && countOrbits < xMax*2
	{
		for (countSquares = 0; countSquares < 2; countSquares++) {
			
			if (xory == 1) {
				x += xdir*countSpirals;
				xdir *= -1;
				loc->setX(x);
				robot->setDestination(loc);
				//genPath(robot);
				//while () {
				//	//wait for robot to catch up
				//}
			}
			else {
				y += ydir*countSpirals;
				ydir *= -1;
				loc->setY(y);
				robot->setDestination(loc);
				//genPath(robot);
				//while () {
				//	//wait for robot to catch up
				//}
			}
			
			xory *= -1; //change dir (left turn)
			
		}
		
		countSpirals++;
	}
	
}

