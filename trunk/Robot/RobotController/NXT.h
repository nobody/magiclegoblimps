#ifndef NXT_H
#define NXT_H

#include <string>

#include "NXTBluetooth/nxt.h"

class NXT
{
public:
	NXT(int port);

	bool Connect();
	void Disconnect();
	
	void Drive();
	void TurnLeft();
	void TurnRight();
	void Stop();

	void FollowLine();

	void PanLeft();
	void PanRight();

	void SetPort(int port);

	int GetBatteryLevel();
	void SendMessage(string message, int box);
	string GetMessage(int box, bool remove);
	void StartProgram(string name);
	void StopPrograms();

private:
	static const int DRIVE_SPEED = 20;
	static const int TURN_SPEED = 30;
	static const int PAN_SPEED = 10;

	int bluetoothPort_;

	Connection* connection_;
	Brick* brick_;
	Motor* motorA_;
	Motor* motorB_;
	Motor* motorC_;
	Sensor* lSensorL_;
	Sensor* lSensorR_;
};

#endif