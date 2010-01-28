#include <cstdlib>
#include <iostream>
#include <string>
#include <conio.h>
#include "NXTCommLib/nxt.h"

#define DRIVE_SPEED 85
#define TURN_SPEED 30
#define PAN_SPEED 10

#define BLUETOOTH_PORT 6

Connection* connection = new Bluetooth();
Brick* nxt = new Brick(connection);
Motor* motorA = new Motor(OUT_A, connection);
Motor* motorB = new Motor(OUT_B, connection);
Motor* motorC = new Motor(OUT_C, connection);
Sensor* lSensorL = new Light(IN_3, connection, LED_ON);
Sensor* lSensorR = new Light(IN_2, connection, LED_ON);

int main()
{
	try
	{
		cout << "Trying to connect to the NXT..." << endl;
		connection->connect(BLUETOOTH_PORT);
		cout << "Connected!" << endl;
		cout << "Battery Level: " << nxt->get_battery_level() << endl;

		char in = ' ';

		while (in != 'q')
		{
			in = getch();

			if (in == 'w')
			{
				motorA->on(DRIVE_SPEED);
				motorC->on(DRIVE_SPEED);
			}
			else if (in == 's')
			{
				motorA->on(-DRIVE_SPEED);
				motorC->on(-DRIVE_SPEED);
			}
			else if (in == 'a')
			{
				motorA->on(TURN_SPEED);
				motorC->on(-TURN_SPEED / 2);
			}
			else if (in == 'd')
			{
				motorA->on(-TURN_SPEED / 2);
				motorC->on(TURN_SPEED);
			}
			else if (in == 'z')
				motorB->on(PAN_SPEED);
			else if (in == 'c')
				motorB->on(-PAN_SPEED);
			else
			{
				motorA->off();
				motorB->stop();
				motorC->off();
			}

			cout << "Left Light Sensor: " << lSensorL->read() << endl;
			cout << "Right Light Sensor: " << lSensorR->read() << endl;
		}

		lSensorL->set(LED_OFF);
		lSensorR->set(LED_OFF);

		connection->disconnect();
	}
	catch (Nxt_exception& e)
	{
		cout << e.what() << endl;
		cout << "error code: " << e.error_code() << endl;
		cout << "error type: " << e.error_type() << endl;
		cout << e.who() << endl;
		connection->disconnect();
	}

	system("pause");
	return 0;
}