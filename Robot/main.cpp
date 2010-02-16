#include <string>
#include <conio.h>
#include "Camera.h"
#include "NXT.h"
#include "Robot.h"

using namespace std;

Robot* robot;
//Robot* robot2;

int main()
{
	robot = new Robot(6, "192.168.1.100", true);
	//robot2 = new Robot(9, "192.168.1.100", true);

	if (robot->GetCamEnabled())
		robot->GetCamera()->StartDisplay();

	cout << "CONNECTED!" << endl;

	while (true)
	{
		int c;

		if (robot->GetCamEnabled())
			robot->GetCamera()->DisplayFrame();

		if (robot->GetCamEnabled())
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

		if (robot->GetNXTEnabled())
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
			else if (c == 'l')
				robot->GetNXT()->FollowLine();
			else if (c == 'x')
				robot->GetNXT()->Stop();
		}

		if (c == 'q')
			break;
	}
	
	if (robot->GetNXTEnabled())
		robot->GetNXT()->Disconnect();
	if (robot->GetCamEnabled())
		robot->GetCamera()->StopDisplay();

	return 0;
}