#include "NXT.h"

NXT::NXT(int port)
{
	bluetoothPort_ = port;
}

NXT::~NXT()
{
	delete connection_;
	delete brick_;
}

bool NXT::Connect()
{
	connection_ = new Bluetooth();
	brick_ = new Brick(connection_);

	try
	{
		connection_->connect(bluetoothPort_);
	}
	catch (Nxt_exception& e)
	{
		connection_->disconnect();
		return false;
	}

	return true;
}

void NXT::Disconnect()
{
	StopPrograms();

	connection_->disconnect();
}

string NXT::GetAddress()
{
	Device_info info;	
	brick_->get_device_info(info);

	return info.bt_address;
}

void NXT::SendMessage(string message)
{
	brick_->write_msg(message, OUT_MAILBOX, false);
}

void NXT::SendMessage(string message, int box)
{
	brick_->write_msg(message, box, false);
}

string NXT::ReadMessage()
{
	return brick_->read_msg(IN_MAILBOX, true); 
}

void NXT::StartProgram(string name)
{
	brick_->start_program(name, false);
}

void NXT::StopPrograms()
{
	brick_->stop_programs(false);
}

void NXT::SetPort(int port)
{
	bluetoothPort_ = port;
}