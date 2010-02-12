#include <string>
#include <conio.h>
#include "Camera.h"
#include "NXT.h"
#include "Robot.h"

using namespace std;

Robot* robot;
int port = 6;
string ip = "192.168.1.100";

int main()
{
	robot = new Robot(port, ip, true);

	if (robot->GetCamEnabled())
		robot->GetCamera()->StartDisplay();

	while (true)
	{
		int c;

		if (robot->GetCamEnabled())
			robot->GetCamera()->DisplayFrame();

		c = cvWaitKey(10);

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