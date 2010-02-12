#include "NXT.h"

NXT::NXT(int port)
{
	bluetoothPort_ = port;
}

bool NXT::Connect()
{
	connection_ = new Bluetooth();
	brick_ = new Brick(connection_);
	motorA_ = new Motor(OUT_A, connection_);
	motorB_ = new Motor(OUT_B, connection_);
	motorC_ = new Motor(OUT_C, connection_);
	lSensorL_ = new Light(IN_3, connection_, LED_OFF);
	lSensorR_ = new Light(IN_2, connection_, LED_OFF);

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
	lSensorL_->set(LED_OFF);
	lSensorR_->set(LED_OFF);

	motorA_->stop();
	motorB_->stop();
	motorC_->stop();

	connection_->disconnect();
}

void NXT::Drive()
{
	motorA_->on(DRIVE_SPEED);
	motorC_->on(DRIVE_SPEED);
}

void NXT::TurnLeft()
{
	motorA_->on(TURN_SPEED);
	motorC_->on(-TURN_SPEED / 2);
}

void NXT::TurnRight()
{
	motorA_->on(-TURN_SPEED / 2);
	motorC_->on(TURN_SPEED);
}

void NXT::Stop()
{
	motorA_->stop();
	motorB_->stop();
	motorC_->stop();
}

void NXT::FollowLine()
{
	lSensorL_->set(LED_ON);
	lSensorR_->set(LED_ON);

	int pos, left, right;

    for(int i = 0; i < 500; i++)
    {
        pos = lSensorR_->read() - lSensorL_->read();
        left = DRIVE_SPEED - pos / 5;
        right = DRIVE_SPEED + pos / 5;
        motorA_->on(right);
        motorC_->on(left);
    }

    motorA_->off();
	motorB_->stop();
    motorC_->off();
}

void NXT::PanLeft()
{
	motorB_->on(PAN_SPEED);
}

void NXT::PanRight()
{
	motorB_->on(-PAN_SPEED);
}

int NXT::GetBatteryLevel()
{
	return brick_->get_battery_level();
}

void NXT::SendMessage(string message, int box)
{
	brick_->write_msg(message, box, false);
}

string NXT::GetMessage(int box, bool remove)
{
	return brick_->read_msg(box, remove); 
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