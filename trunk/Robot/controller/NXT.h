#ifndef NXT_H
#define NXT_H

#include <string>

#include "NXTBluetooth/nxt.h"

class NXT
{
public:
	NXT(int port);
	~NXT();

	bool Connect();
	void Disconnect();
	
	int GetPort() { return bluetoothPort_; }
	void SetPort(int port);

	string GetAddress();

	void SendMessage(string message);
	string ReadMessage();

	void StartProgram(string name);
	void StopPrograms();

private:
	static const int IN_MAILBOX = 0;
	static const int OUT_MAILBOX = 1;

	int bluetoothPort_;

	Connection* connection_;
	Brick* brick_;
};

#endif