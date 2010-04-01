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

	//need to retrieve the list of known objects from server on connect

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
			//potentially needs to decide if recieved data is a command,
			//robot, or object - but may not be required if we're only
			//grabbing the list of objects from the server once on connection

			//also need to do something about null/terminating characters

			cout << "COMMAND FROM SERVER: " << recvBuf << endl;
			string command = recvBuf;
			Command(command);
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

bool Controller::InitRobot()
{
	//send inital robot information on add
	return true;
}

bool Controller::SendRobots()
{
	//send information about all our robots to the server
	return true;
}

bool Controller::SendObject()
{
	//send information about all our objects to the server
	return true;
}

bool Controller::RecieveObjects()
{
	//recieve all saved objects from server
	return true;
}

bool Controller::Command(string command)
{
	//should translate command from server format into our command format
	//then send it on to the requested robot

	//temporary format (needs discussing with server group)
	//[command]$[robotnumber]$[subcommand]$[value]

	vector<string> tokens;
	tokenize(command, tokens, "$");

	if (tokens.size() == 0)
		return false;

	//example of command translation (not final)
	if (tokens[0].compare("target") == 0)
	{
		robots_[atoi(tokens[1].c_str())]->ExecuteCommand("target" + tokens[2]);
	}

	return true;
}

//after creating a server and connecting a client, can be used to send
//test commands as if they were from the server
bool Controller::TestServer(string command)
{
	int bufLength = command.length();
	int iResult = 0;

	iResult = send(serverSocket_, command.c_str(), bufLength, 0);
    if (iResult == SOCKET_ERROR) 
	{
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(serverSocket_);
        WSACleanup();
        return false;
    }
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

		timer_ = 0;
	}
}