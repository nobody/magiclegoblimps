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
			controller->ConnectToServer(ip);
		}
	}

	//testconnect
	else if (tokens[0].compare("testconnect") == 0)
	{
		controller->ConnectToServer("129.110.116.66");
	}

	//testrobot
	else if (tokens[0].compare("testrobot") == 0)
	{
		//lazy connection for repeat testing
		Robot* robot = new Robot(1, "192.168.1.101", true);
		robot->Connect();
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
				robot->Connect();
				controller->AddRobot(robot);
			}
			else if (tokens[3].compare("false") == 0)
			{
				robot = new Robot(port, ip, false);
				robot->Connect();
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

			controller->GetRobot(
				atoi(tokens[1].c_str()))->ExecuteCommand(command);
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

	//removeobject id
	else if (tokens[0].compare("removeobject") == 0)
	{
		if (tokens.size() == 2)
		{
			vector<TrackingObject*>::iterator it;

			for (it = Camera::GetTrackableObjects().begin(); 
				it != Camera::GetTrackableObjects().end(); it++)
			{
				if ((*it)->GetID() == atoi(tokens[1].c_str()))
				{
					delete (*it);
					Camera::GetTrackableObjects().erase(it);
					break;
				}
			}
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
	controller = new Controller(2, 2);

	cout << "ROBOT CONTROLLER" << endl;
	cout << "----------------" << endl;

	while (running)
	{       
		//this might need threading so it doesn't block
		//we're not inputting much, but it halts pretty much everything
		//until input is completed
		if (_kbhit())
			LocalInput();

		controller->Update();

		int c = cvWaitKey(10);

		vector<Robot*> robots = controller->GetRobotVector();
		vector<Robot*>::iterator it;

		for (it = robots.begin(); it != robots.end(); it++)
		{
			(*it)->GetCamera()->Update();

			if ((*it)->GetCamera()->GetLocalDisplay())
			{
				if (c != -1)
					(*it)->GetCamera()->SendKey(c);

				(*it)->GetCamera()->DisplayFrame();
			}
		}
	}

	controller->Disconnect();

	delete controller;

	return 0;
}