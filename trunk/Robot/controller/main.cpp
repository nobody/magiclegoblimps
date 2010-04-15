#include <string>
#include <conio.h>

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
	//all of this needs serious error checking and cleaning up

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
	//look into using a STL map instead

	//quit
	if (tokens[0].compare("quit") == 0)
		running = false;

	//connect ip
	if (tokens[0].compare("connect") == 0)
	{
		string ip = tokens[1];
		controller->ConnectToServer(ip);
	}

	//serve
	if (tokens[0].compare("serve") == 0)
	{
		controller->Serve();
	}

	//testserver command
	if (tokens[0].compare("testserver") == 0)
	{
		controller->TestServer(atoi(tokens[1].c_str()));
	}

	//testrobot
	if (tokens[0].compare("testrobot") == 0)
	{
		//lazy connection for repeat testing
		Robot* robot = new Robot(6, "192.168.1.100", false);

		robot->Connect();

		controller->AddRobot(robot);
	}

	//addrobot port/id ip true/false
	if (tokens[0].compare("addrobot") == 0)
	{
		int port = atoi(tokens[1].c_str());
		string ip = tokens[2];

		Robot* robot;

		if (tokens[3].compare("true") == 0)
			robot = new Robot(port, ip, true);
		else if (tokens[3].compare("false") == 0)
			robot = new Robot(port, ip, false);

		robot->Connect();

		controller->AddRobot(robot);
	}

	//removerobot port/id
	if (tokens[0].compare("removerobot") == 0)
	{
		int port = atoi(tokens[1].c_str());

		controller->RemoveRobot(port);
	}

	//display port/id
	if (tokens[0].compare("display") == 0)
	{
		controller->GetRobot(
			atoi(tokens[1].c_str()))->GetCamera()->StartDisplay();
	}

	//stopdisplay port/id
	if (tokens[0].compare("stopdisplay") == 0)
	{
		controller->GetRobot(
			atoi(tokens[1].c_str()))->GetCamera()->StopDisplay();
	}

	//startprogram port/id name
	if (tokens[0].compare("startprogram") == 0)
	{
		controller->GetRobot(atoi(tokens[1].c_str()))->GetNXT()->StartProgram(tokens[2]);
	}

	//stopprograms port/id
	if (tokens[0].compare("stopprograms") == 0)
	{
		controller->GetRobot(atoi(tokens[1].c_str()))->GetNXT()->StopPrograms();
	}

	//test port/id command
	if (tokens[0].compare("test") == 0)
	{
		string command = tokens[2];

		for (int i = 3; i < tokens.size(); i++)
			command += " " + tokens[i];

		controller->GetRobot(
			atoi(tokens[1].c_str()))->ExecuteCommand(command);
	}

	//testbox
	if (tokens[0].compare("testbox") == 0)
	{
		CvBox2D testBox;
		testBox.angle = 30;
		testBox.center.x = 4;
		testBox.center.y = 2;
		testBox.size.height = 10;
		testBox.size.width = 5;

		char* testArr = TrackingObject::BoxToArray(testBox);
		CvBox2D newBox = TrackingObject::ArrayToBox(testArr);

		cout << newBox.angle << endl;
		cout << newBox.center.x << endl;
		cout << newBox.center.y << endl;
		cout << newBox.size.height << endl;
		cout << newBox.size.width << endl;

		delete testArr;
	}
}

int main(int argc, char* argv[])
{
	controller = new Controller();

	cout << "ROBOT CONTROLLER" << endl;
	cout << "----------------" << endl;

	while (running)
	{	
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