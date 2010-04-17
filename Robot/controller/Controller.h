#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <process.h>
#include <time.h>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <queue>
#include <process.h>

#include "Path.h"
#include "Robot.h"
#include "Tokenizer.h" 
#include "TrackingObject.h"

#include "Server/protocol.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std;

class Controller
{
public:
	Controller();
	Controller(int xDim, int yDim);

	bool ConnectToServer(string ip);

	bool Serve();

	static void ClientThread(void* params);

	static bool Command(int id, int command, int arg);
	bool TestServer(int type);

	void AddRobot(Robot* robot);
	Robot* GetRobot(int id);
	void RemoveRobot(int id);

	vector<Robot*> GetRobotVector();

	void Disconnect();

	Path* genPath(Robot& robot);
	vector<GridLoc*> getValidMoves(GridLoc loc, 
		vector<GridLoc*> illMoves);
	vector<GridLoc*> getIllMoves();

	void Update();

private:
	static const int POLL_INTERVAL = 10;
	static const int BUFFER_LENGTH = 512;

	char* port_;
	int xMax;
	int yMax;

	bool connected_;

	vector<GridLoc*> permIllegalLocs;
	//vector<Robot*> robots_;

	static vector<Robot*> robots_;

	float timer_;
	time_t lastTime_;

	static SOCKET connectSocket_;
	SOCKET serverSocket_;
};

#endif