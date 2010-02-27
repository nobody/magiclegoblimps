#include <string>
#include <conio.h>
#include <process.h>

#include "Tokenizer.h"

#include "Camera.h"
#include "Controller.h"
#include "NXT.h"
#include "Robot.h"

using namespace std;

Controller* controller;

bool running = true;

void UpdateLoop(void* params)
{
	while (running)
	{
		controller->Update();
	}

	_endthread();
}

void LocalDisplay(void* params)
{
	while (running)
	{
		vector<Robot*> robots = controller->GetRobotVector();
		vector<Robot*>::iterator it;

		for (it = robots.begin(); it != robots.end(); it++)
		{
			if ((*it)->GetCamConnected())
				(*it)->GetCamera()->DisplayFrame();
		}

		cvWaitKey(10);
	}

	_endthread();
}

void LocalInput()
{
	//handle input on local machine

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

	//quit
	if (tokens[0].compare("quit") == 0)
		running = false;

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
		string command = "";

		for (int i = 2; i < tokens.size(); i++)
			command += " " + tokens[i];

		controller->GetRobot(
			atoi(tokens[1].c_str()))->ExecuteCommand(command);
	}

	//testserver command
	if (tokens[0].compare("testserver") == 0)
	{
		string command = "";

		for (int i = 1; i < tokens.size(); i++)
			command += " " + tokens[i];

		controller->TestCommand(command);
	}
}

int main(int argc, char* argv[])
{
	//grab server IP from args

	controller = new Controller();

	_beginthread(UpdateLoop, 0, NULL);
	_beginthread(LocalDisplay, 0, NULL);

	cout << "ROBOT CONTROLLER" << endl;
	cout << "----------------" << endl;

	while (running)
	{	
		if (kbhit())
			LocalInput();
	}

	return 0;
}