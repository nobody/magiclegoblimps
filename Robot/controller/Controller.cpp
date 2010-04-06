#include "Controller.h"

//static member variables must be redeclared in source
vector<Robot*> Controller::robots_;
SOCKET Controller::connectSocket_;
//vector<TrackingObject*> Camera::GetTrackableObjects();

Controller::Controller()
{
	timer_ = 0;
	connectSocket_ = NULL;
	serverSocket_ = NULL;
	port_ = "7626";
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

	robotInit* init = new robotInit[robots_.size()];

	for (int i = 0; i < robots_.size(); i++)
	{
		init[i].RID = robots_[i]->GetID();
		if (robots_[i]->GetCamera()->GetDLinkCam())
			init[i].cameraType = P_DLINK;
		else
			init[i].cameraType = P_CISCO;
		//init[i].VideoURL = robots_[i]->GetCamera()->GetVideoURL();
		init[i].x = robots_[i]->GetLocationX();
		init[i].y = robots_[i]->GetLocationY();
	}

	write_data(P_ROBOT_INIT, init, robots_.size(), &sendArray);

	delete[] init;
	delete[] sendArray.array;

	_beginthread(ClientThread, 0, NULL);

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
					robots_[assign[i].RID]->ExecuteCommand(
						"target " + assign[i].OID);
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
	//should translate command from server format into our command format
	//then send it on to the requested robot

	//example of command translation (not actual command)
	if (command == 1)
	{
		robots_[id]->ExecuteCommand("move " + arg);
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

	delete[] sendArray.array;

	return true;
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

	/*
	for (int i = 0; i < Camera.GetTrackableObjects().size(); i++)
	{
		delete Camera.GetTrackableObjects()[i];
		Camera.GetTrackableObjects().erase(
			Camera.GetTrackableObjects().begin() + i);
	}
	*/

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
					cout << "Robot " << (*it)->GetID() << " " <<
						(*it)->GetNXT()->ReadMessage() << 
						endl;
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

		byteArray sendArray;

		robotUpdate* update = new robotUpdate[robots_.size()];

		for (int i = 0; i < robots_.size(); i++)
		{
			update[i].RID = robots_[i]->GetID();
			update[i].x = robots_[i]->GetLocationX();
			update[i].y = robots_[i]->GetLocationY();
			update[i].listSize = 
				robots_[i]->GetCamera()->GetVisibleObjects().size();
			//update[i].objects =
			//update[i].qualities =
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

		timer_ = 0;
	}
}