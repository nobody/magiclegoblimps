#include "Controller.h"

#define PORT "7626"

Controller::Controller()
{
	timer_ = 0;
}

bool Controller::ConnectToServer(string ip)
{
	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	char* sendBuf = "This is a test!";
	char recvBuf[BUFFER_LENGTH];
	int iResult;
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

	iResult = getaddrinfo(ip.c_str(), PORT, &hints, &result);
    if ( iResult != 0 ) 
	{
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return false;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }

        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
		{
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET) 
	{
        printf("Unable to connect to server!\n");
        WSACleanup();
        return false;
    }

    iResult = send(connectSocket, sendBuf, (int)strlen(sendBuf), 0);
    if (iResult == SOCKET_ERROR) 
	{
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return false;
    }

    printf("Bytes Sent: %ld\n", iResult);

    iResult = shutdown(connectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
	{
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return false;
    }

    do 
	{
        iResult = recv(connectSocket, recvBuf, recvBufLen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    } 
	while( iResult > 0 );

    closesocket(connectSocket);
    WSACleanup();

	return true;
}

bool Controller::TestServer()
{
	WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET,
           clientSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    hints;
    char recvBuf[BUFFER_LENGTH];
    int iResult, iSendResult;
    int recvBufLen = BUFFER_LENGTH;
    

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

    iResult = getaddrinfo(NULL, PORT, &hints, &result);
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

    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) 
	{
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    closesocket(listenSocket);

    do 
	{
        iResult = recv(clientSocket, recvBuf, recvBufLen, 0);
        if (iResult > 0) 
		{
            printf("Bytes received: %d\n", iResult);

            iSendResult = send(clientSocket, recvBuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) 
			{
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(clientSocket);
                WSACleanup();
                return false;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else  
		{
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }

    } 
	while (iResult > 0);

    iResult = shutdown(clientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
	{
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    closesocket(clientSocket);
    WSACleanup();

    return true;
}

void Controller::TestCommand(string command)
{
	//should translate command from server format into our command format
	//then send it on to the requested robot

	//temporary format (needs discussing with server group)
	//very likely to change
	//[command]$[robotnumber]$[subcommand]$[value]

	//see main.cpp for (badly implemented) usage

	vector<string> tokens;
	tokenize(command, tokens, "$");

	if (tokens.size() == 0)
		return;
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

	//need to delete trackable objects after disconnect
	/*
	for (int i = 0; i < trackableObjects_.size(); i++)
	{
		delete trackableObjects_[i];
		trackableObjects_.erase(trackableObjects_.begin() + i);
	}
	*/
}

void Controller::Update()
{
	//check for server message and respond

	time_t seconds = time(NULL);
	float interval = seconds - lastTime_;
	lastTime_ = seconds;

	timer_ += interval;

	if (timer_ > POLL_INTERVAL)
	{
		vector<Robot*>::iterator it;

		for (it = robots_.begin(); it != robots_.end(); it++)
		{
			//need to make sure robot is filling mailbox before using this
			/*
			if ((*it)->GetNXTConnected())
			{
				cout << "Robot " << (*it)->GetID() << " " <<
					(*it)->GetNXT()->ReadMessage((*it)->GetNXT()->IN_MAILBOX) << 
					endl;
			}
			*/
		}

		timer_ = 0;
	}
}