#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <process.h>
#include <time.h>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <queue>

#include "Path.h"
#include "Robot.h"
#include "Tokenizer.h" 
#include "TrackingObject.h"

#include "Server/protocol.h"

#pragma comment(lib, "ws2_32.lib")

#define IDLE			2
#define INTERSECTION	8

struct RobotParams
{
	Robot* robot;
};

using namespace std;

class Controller
{
public:
	Controller(int xDim, int yDim, string routerIP);

	bool Connect(string ip);
	static void Disconnect();

	static void ClientThread(void* params);

	static bool Command(int id, int command, int arg);

	static void AddRobot(Robot* robot);
	static Robot* GetRobot(int id);
	static void RemoveRobot(int id);

	static void SendRobot(int id);
	static void SendObject(int id);

	static Path* genPath(Robot& robot);
	static vector<GridLoc*> getValidMoves(GridLoc loc, 
		vector<GridLoc*> illMoves);
	static vector<GridLoc*> getIllMoves();

	static void SearchObject(Robot* robot);
	static void SpiralSearch(Robot* robot, GridLoc* loc);

	static string newCmd(Robot* rob);

	static void UpdateThread(void* params);
	static void RobotThread(void* params);

	static void Stop() { running_ = false; }

private:
	static const int SEND_INTERVAL = 5;
	static const int BUFFER_LENGTH = 512;

	static char* port_;
	static int xMax;
	static int yMax;

	static bool connected_;

	static vector<GridLoc*> permIllegalLocs;

	static vector<Robot*> robots_;

	static float timer_;
	static time_t lastTime_;

	static SOCKET connectSocket_;

	static HANDLE robotSemaphore_;

	static bool running_;
};

#endif