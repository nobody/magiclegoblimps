#include <string>
#include <conio.h>
#include <sstream>

#define _WINSOCKAPI_ 

#include "Tokenizer.h"

#include "Camera.h"
#include "Controller.h"
#include "NXT.h"
#include "Robot.h"

#define SIMON

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

	//serve
	else if (tokens[0].compare("serve") == 0)
	{
		controller->Serve();
	}

	//testserver command
	else if (tokens[0].compare("testserver") == 0)
	{
		if (tokens.size() == 2)
			controller->TestServer(atoi(tokens[1].c_str()));
	}

	//testrobot
	else if (tokens[0].compare("testrobot") == 0)
	{
		//lazy connection for repeat testing
		Robot* robot = new Robot(6, "192.168.1.100", false);
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

	//test port/id command
	else if (tokens[0].compare("test") == 0)
	{
		if (tokens.size() == 3)
		{
			string command = tokens[2];

			for (int i = 3; i < (int)tokens.size(); i++)
				command += " " + tokens[i];

			controller->GetRobot(
				atoi(tokens[1].c_str()))->ExecuteCommand(command);
		}
	}

	//testbox
	else if (tokens[0].compare("testbox") == 0)
	{
		CvBox2D testBox;
		testBox.angle = 30;
		testBox.center.x = 4;
		testBox.center.y = 2;
		testBox.size.height = 10;
		testBox.size.width = 5;

		int size = 0;

		char* testArr = TrackingObject::BoxToArray(testBox, &size);
		CvBox2D newBox = TrackingObject::ArrayToBox(testArr);

		cout << "Size: " << size << endl;

		cout << newBox.angle << endl;
		cout << newBox.center.x << endl;
		cout << newBox.center.y << endl;
		cout << newBox.size.height << endl;
		cout << newBox.size.width << endl;

		delete testArr;
	}

	//testhist objectid
	else if (tokens[0].compare("testhist") == 0)
	{
		if (tokens.size() == 2)
		{
			int size = 0;

			char* testArr = TrackingObject::HistogramToArray(
				Camera::GetTrackableObjects()[atoi(tokens[1].c_str())]->GetHistogram(), &size);

			CvHistogram* testHist = TrackingObject::ArrayToHistogram(testArr);

			cout << "Size: " << size << endl;

			Camera::GetTrackableObjects()[atoi(tokens[1].c_str())]->SetHistogram(testHist);

			delete testArr;
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
	//BEGIN Simon's Camera Test Code
	//------------------------------------------------------>
#ifdef SIMON
	controller = new Controller(2, 2);

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
#else

	//<------------------------------------------------------
	//END Simon's Camera Test Code


	//Begin my test code
	//------------------------------------------------------>
	//Init Controller with 10x10 grid
	//TODO:
	//Update robot location needs fixing
	//Setting targetVisible
	//Setting robotMoving flag
	controller = new Controller(10, 10);

	//Create robot, add it to controller
	Robot* robbie = new Robot(0, "0.0.0.0", false);
	//Robot* robbi2 = new Robot(1, "0.0.0.0", false);
	//Robot* robbi3 = new Robot(2, "0.0.0.0", false);
	//Robot* robbi4 = new Robot(3, "0.0.0.0", false);
	controller->AddRobot(robbie);
	//controller->AddRobot(robbi2);
	//controller->AddRobot(robbi3);
	//controller->AddRobot(robbi4);

	//Test vals in robot
	//robbi2->getLocation()->setX(1);
	//robbi3->getLocation()->setY(1);
	//robbi4->getLocation()->setX(1);
	//robbi4->getLocation()->setY(1);

	//Set destination
	robbie->setDestination(new GridLoc(1, 1));
	//robbi2->setDestination(new GridLoc(5, 4));
	//robbi3->setDestination(new GridLoc(5, 6));
	//robbi4->setDestination(new GridLoc(5, 7));

	//Generate Path
	//robbi2->setPath(controller->genPath(*robbi2));
	//robbi3->setPath(controller->genPath(*robbi3));
	//robbi4->setPath(controller->genPath(*robbi4));
	robbie->setPath(controller->genPath(*robbie));
	//robbi2->getPath()->print();
	//robbi3->getPath()->print();
	//robbi4->getPath()->print();
	//if(robbie->getPath())
	robbie->getPath()->print();

	vector<Robot*> robs = controller->GetRobotVector();
	vector<Robot*>::iterator it;
	for(it = robs.begin(); it != robs.end(); it++)
	{		
		printf("Robot: %d - (%d, %d, %d)->(%d, %d)\n", (*it)->getID(),
			(*it)->getLocation()->getX(), 
			(*it)->getLocation()->getY(),
			(*it)->getHeading(),
			(*it)->getPath()->getStart()->getX(),
			(*it)->getPath()->getStart()->getY());
		(*it)->getPath()->print();
		printf("\n");
	}
	for(int i = 0; i < 5; i++)
	{
		for(it = robs.begin(); it != robs.end(); it++)
		{	
			(*it)->SetUpdate((*it)->getLocation()->getX(),
				(*it)->getLocation()->getY(),
				(*it)->getHeading(), 0, 100, 0);
			(*it)->newCmd();	
			printf("Robot: %d - (%d, %d, %d)->(%d, %d)\n", (*it)->getID(),
				(*it)->getLocation()->getX(), 
				(*it)->getLocation()->getY(),
				(*it)->getHeading(),
				(*it)->getPath()->getStart()->getX(),
				(*it)->getPath()->getStart()->getY());
			(*it)->getPath()->print();
			printf("\n");
		}
	}
	_getch();
	
	return 0;
#endif
}