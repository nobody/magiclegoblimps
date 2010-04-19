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

#define IDLE			1
#define INTERSECTION	8
#define STOP			1024

using namespace std;

class Controller
{
public:
	Controller(int xDim, int yDim);

	bool ConnectToServer(string ip);

	static void ClientThread(void* params);

	static bool Command(int id, int command, int arg);

	void AddRobot(Robot* robot);
	Robot* GetRobot(int id);
	void RemoveRobot(int id);

	vector<Robot*> GetRobotVector();

	void Disconnect();

	Path* genPath(Robot& robot);
	vector<GridLoc*> getValidMoves(GridLoc loc, 
		vector<GridLoc*> illMoves);
	vector<GridLoc*> getIllMoves();

	void SearchObject(int robotID, int objID, GridLoc* lastKnownLoc);
	void SpiralSearch(Robot* robot, Camera& camera, GridLoc* loc);

	void Update();

private:
	static const int POLL_INTERVAL = 5;
	static const int BUFFER_LENGTH = 512;

	char* port_;
	int xMax;
	int yMax;

	bool connected_;

	vector<GridLoc*> permIllegalLocs;

	static vector<Robot*> robots_;

	int lastObjectSize_;
	int lastRobotSize_;

	float timer_;
	time_t lastTime_;

	static SOCKET connectSocket_;

	//internal static versions for use in static thread
	static Robot* getRobot(int id);
	static void disconnect();
};

#endif