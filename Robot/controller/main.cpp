#include <string>
#include <conio.h>
#include <sstream>

#define _WINSOCKAPI_ 

#include "Tokenizer.h"

#include "Camera.h"
#include "Controller.h"
#include "NXT.h"
#include "Robot.h"

using namespace std;

Controller* controller;

bool running = true;

void LocalInput()
{
	//could probably use some more input validation
	//maybe the functions should check their input internally

	cout << "INPUT: ";

	string line;
	getline(cin, line);

	if (line.compare("") == 0)
		return;

	vector<string> tokens;
	tokenize(line, tokens, " ");

	if (tokens.size() == 0)
		return;

	//if statements are ugly, but can't switch strings

	//quit
	if (tokens[0].compare("quit") == 0)
		running = false;

	//connect ip
	else if (tokens[0].compare("connect") == 0)
	{
		if (tokens.size() == 2)
		{
			string ip = tokens[1];
			controller->Connect(ip);
		}
	}

	//testconnect
	else if (tokens[0].compare("testconnect") == 0)
	{
		controller->Connect("10.176.14.94");
	}

	//testrobot
	else if (tokens[0].compare("testrobot") == 0)
	{
		//lazy connection for repeat testing
		Robot* robot = new Robot(9, "192.168.1.101:7001", true);
		controller->AddRobot(robot);

		robot = new Robot(6, "192.168.1.103:7003", true);
		controller->AddRobot(robot);
	}

	//addrobot port/id ip true/false
	else if (tokens[0].compare("addrobot") == 0)
	{
		if (tokens.size() == 4)
		{
			int port = atoi(tokens[1].c_str());
			string ip = tokens[2];

			Robot* robot;

			if (tokens[3].compare("true") == 0)
			{
				robot = new Robot(port, ip, true);
				controller->AddRobot(robot);
			}
			else if (tokens[3].compare("false") == 0)
			{
				robot = new Robot(port, ip, false);
				controller->AddRobot(robot);
			}
		}
	}

	//removerobot port/id
	else if (tokens[0].compare("removerobot") == 0)
	{
		if (tokens.size() == 2)
		{
			int port = atoi(tokens[1].c_str());

			controller->RemoveRobot(port);
		}
	}

	//command port/id command
	else if (tokens[0].compare("command") == 0)
	{
		if (tokens.size() > 2)
		{
			string command = tokens[2];

			for (int i = 3; i < (int)tokens.size(); i++)
				command += " " + tokens[i];

			Robot* robot = controller->GetRobot(atoi(tokens[1].c_str()));

			robot->ExecuteCommand(command);
		}
	}

	//display port/id
	else if (tokens[0].compare("display") == 0)
	{
		if (tokens.size() == 2)
		{
			if (controller->GetRobot(
				atoi(tokens[1].c_str()))->GetCamConnected())
				controller->GetRobot(
				atoi(tokens[1].c_str()))->GetCamera()->StartDisplay();
		}
	}

	//stopdisplay port/id
	else if (tokens[0].compare("stopdisplay") == 0)
	{
		if (tokens.size() == 2)
		{
			if (controller->GetRobot(
				atoi(tokens[1].c_str()))->GetCamConnected())
				controller->GetRobot(
				atoi(tokens[1].c_str()))->GetCamera()->StopDisplay();
		}
	}

	//startprogram port/id name
	else if (tokens[0].compare("startprogram") == 0)
	{
		if (tokens.size() == 2)
			controller->GetRobot(atoi(tokens[1].c_str()))->GetNXT()->StartProgram(tokens[2]);
	}

	//stopprograms port/id
	else if (tokens[0].compare("stopprograms") == 0)
	{
		if (tokens.size() == 2)
			controller->GetRobot(atoi(tokens[1].c_str()))->GetNXT()->StopPrograms();
	}

	//saveobject port/id
	else if (tokens[0].compare("saveobject") == 0)
	{
		int id = 0;
		if (tokens.size() == 2)
			id = controller->GetRobot(atoi(tokens[1].c_str()))->GetCamera()->SaveObject();

		controller->SendObject(id);
	}
	
	//removeobject id
	else if (tokens[0].compare("removeobject") == 0)
	{
		if (tokens.size() == 2)
		{
			Camera::RemoveTrackingObject(atoi(tokens[1].c_str()));
		}
	}

	//testmove id x y
	else if (tokens[0].compare("testmove") == 0)
	{	
		GridLoc* loc = 
			new GridLoc(atoi(tokens[2].c_str()), atoi(tokens[3].c_str()));

		controller->GetRobot(atoi(tokens[1].c_str()))->setDestination(loc);
		controller->GetRobot(atoi(tokens[1].c_str()))->setPath(controller->genPath(*controller->GetRobot(atoi(tokens[1].c_str()))));
	}

	//default
	else
	{
		cout << "Invalid Command" << endl;
	}
}

int main(int argc, char* argv[])
{
	/*
	if (argc != 3)
	{
		cout << "Incorrect number of arguments" << endl;
		cout << "Usage: controller.exe <gridx> <gridy> <routerip>" << endl;
	}

	controller = new Controller(atoi(argv[0]), atoi(argv[1]), argv[2]);
	*/

	controller = new Controller(5, 4, "10.110.95.228");

	cout << "ROBOT CONTROLLER" << endl;
	cout << "----------------" << endl;

	while (running)
	{       
		if (_kbhit())
			LocalInput();

		cvWaitKey(10);
	}

	controller->Stop();
	controller->Disconnect();

	delete controller;

	return 0;
}