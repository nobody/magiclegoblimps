#include "Controller.h"

//static member variables must be redeclared in source
vector<Robot*> Controller::robots_;
SOCKET Controller::connectSocket_;

Controller::Controller()
{
	timer_ = 0;
	connectSocket_ = NULL;
	serverSocket_ = NULL;
	port_ = "7626";

	connected_ = false;
}

Controller::Controller(int xDim, int yDim)
{
	timer_= 0;
	xMax = xDim;
	yMax = yDim;
}

bool Controller::ConnectToServer(string ip)
{
	WSADATA wsaData;
	connectSocket_ = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	char recvBuf[BUFFER_LENGTH];
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
		init[1].RID = -1;
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
			init[i].VideoURL = &(*it)->GetCamera()->GetVideoURL();
			init[i].x = (*it)->GetLocationX();
			init[i].y = (*it)->GetLocationY();

			i++;
		}
	}

	write_data(P_ROBOT_INIT, init, robots_.size(), &sendArray);

	delete[] init;
	delete[] sendArray.array;

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
				object* obj = new object[data->size];
				obj = (object*)data->array;

				for (int i = 0; i < data->size; i++)
				{
					cout << "Adding Object " << obj[i].OID << 
						" : " << obj[i].name << endl;

					//TrackingObject* newObj = TrackingObject(
					//add to the tracking objects vector
				}
			}
			else if (type == P_ASSIGNMENT)
			{
				assignment* assign = new assignment[data->size];
				assign = (assignment*)data->array;

				for (int i = 0; i < data->size; i++)
				{
					cout << "Assigning Robot " << assign[i].RID << 
						" to Object " << assign[i].OID << endl;
					/*
					GetRobot(assign[i].RID)->ExecuteCommand("target " + 
						assign[i].OID);
					*/
				}

				delete[] assign;
			}
			else if (type == P_COMMAND)
			{
				command* comm = new command[data->size];
				comm = (command*)data->array;

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
	if (command == 1)
	{
		//GetRobot(id)->ExecuteCommand("forward " + arg);
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

	delete[] sendArray.array;

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
		}
	}
}

vector<Robot*> Controller::GetRobotVector()
{
	return robots_;
}

void Controller::Disconnect()
{
	vector<Robot*>::iterator it;

	for (it = robots_.begin(); it != robots_.end(); it++)
	{
		(*it)->Disconnect();
		delete (*it);
		robots_.erase(it);
	}

	vector<TrackingObject*>::iterator oit;

	for (oit = Camera::GetTrackableObjects().begin(); oit != 
		Camera::GetTrackableObjects().end(); oit++)
	{
		delete (*oit);
		Camera::GetTrackableObjects().erase(oit);
	}

	if (connectSocket_ != NULL)
	{
		//socket might need shutdown
		closesocket(connectSocket_);
	    WSACleanup();
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
	float interval = seconds - lastTime_;
	lastTime_ = seconds;

	timer_ += interval;

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
				}
				catch (Nxt_exception& e)
				{
					cout << e.what() << endl;
					cout << e.error_code() << endl;
					cout << e.error_type() << endl;
					cout << e.who() << endl;
				}
			}
		}

		if (connected_)
		{
			byteArray sendArray;

			robotUpdate* update = new robotUpdate[robots_.size()];

			int i = 0;

			for (it = robots_.begin(); it != robots_.end(); it++)
			{
				update[i].RID = (*it)->GetID();
				update[i].x = (*it)->GetLocationX();
				update[i].y = (*it)->GetLocationY();
				update[i].listSize = 
					(*it)->GetCamera()->GetVisibleObjects().size();

				int* objects = new int[update[i].listSize];
				for (int i = 0; i < update[i].listSize; i++)
				{
					objects[i] = 
						(*it)->GetCamera()->GetVisibleObjects()[i]->GetID();
				}
				update[i].objects = objects;

				int* qualities = new int[update[1].listSize];
				for (int i = 0; i < update[i].listSize; i++)
				{
					qualities[i] = 
						(*it)->GetCamera()->GetVisibleObjects()[i]->GetQuality();
				}
				update[i].qualities = qualities;

				i++;
			}

			write_data(P_ROBOT_UPDATE, update, robots_.size(), &sendArray);

			delete[] update;

			int iResult = 0;

			iResult = send(serverSocket_, sendArray.array, sendArray.size, 0);
		    if (iResult == SOCKET_ERROR) 
			{
		        printf("send failed: %d\n", WSAGetLastError());
		        closesocket(serverSocket_);
		        WSACleanup();
		    }

			delete[] sendArray.array;
		}

		timer_ = 0;
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
	for(int i = 0; i < illMoves.size(); i++)
	{
		if(illMoves[i] == robot.getDestination())
			illMoves.erase(illMoves.begin()+i);
	}

	Path* pth;
	for(int i = 0; i < moves.size(); i++)
	{
		pth = new Path();
		pth->extend(moves[i]);
		pth->calcMetric(*robot.getLocation(), 
			*robot.getDestination());
		pathHeap.push(pth);
		illMoves.push_back(moves[i]);
	}

	Path* best = pathHeap.top();
	pathHeap.pop();

	if(best != NULL)
	{
		while(*best->getEnd() != *robot.getDestination())
		{
			moves = getValidMoves(*best->getEnd(), illMoves);
			for(int i = 0; i < moves.size(); i++)
			{
				Path* newPath = best->copy();
				newPath->extend(moves[i]);
				newPath->calcMetric(*robot.getLocation(),
					*robot.getDestination());
				pathHeap.push(newPath);
				illMoves.push_back(moves[i]);
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

	//vector<GridLoc*> validMoves;
	for(int i = 0; i < illMoves.size(); i++)
	{
		int j = 0;
		while(j < moves.size())
		{
			if(*moves[j] == *illMoves[i])
			{
				moves.erase(moves.begin()+j);
				break;
			}
			else
			{
				j++;
			}
		}
	}

	return moves;
}

vector<GridLoc*> Controller::getIllMoves()
{
	vector<GridLoc*> illMoves;

	for(int i = 0; i < robots_.size(); i++)
	{
		GridLoc* robLoc = 
			new GridLoc(robots_[i]->getLocation()->getX(),
			robots_[i]->getLocation()->getY());
		illMoves.push_back(robLoc);
	}
	return illMoves;
}