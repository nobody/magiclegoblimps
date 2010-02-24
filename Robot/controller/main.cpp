#include <string>
#include <conio.h>

#include "Camera.h"
#include "Controller.h"
#include "NXT.h"
#include "Robot.h"

using namespace std;

Robot* robot;

int main()
{
	//robot = new Robot(6, "192.168.1.100", true);
	robot = new Robot(6, "192.168.1.102", false);

	while (true)
	{
		int c;

		if (robot->GetCamConnected() && robot->GetCamera()->GetLocalDisplay())
			robot->GetCamera()->DisplayFrame();

		if (robot->GetCamConnected())
		{
			//this doesn't seem work when camera isn't connected
			//need to add non-blocking alternative for testing
			//deployment won't be waiting for direct input (use threads?)
			c = cvWaitKey(10);
		}
		else
		{
			cout << "Key:" << endl;
			c = getch();
		}

		if (robot->GetNXTConnected())
		{
			if (c == 'w')
				robot->GetNXT()->Drive();
			else if (c == 'a')
				robot->GetNXT()->TurnLeft();
			else if (c == 'd')
				robot->GetNXT()->TurnRight();
			else if (c == 'z')
				robot->GetNXT()->PanLeft();
			else if (c == 'c')
				robot->GetNXT()->PanRight();
			else if (c == 'x')
				robot->GetNXT()->Stop();
		}

		if (c == 'q')
			break;
	}
	
	robot->Disconnect();

	return 0;
}