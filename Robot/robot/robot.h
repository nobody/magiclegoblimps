// mailboxes
#define OUTBOX 0  // to controller
#define INBOX 1   // from controller
#define CAMERABOX 2 // if needed

// motors
#define MOTOR_LEFT OUT_A
#define MOTOR_RIGHT OUT_C
#define MOTOR_BOTH OUT_AC
#define MOTOR_CAMERA OUT_B

#define SPEED 30
#define SPEED_ADJUST 4
#define SPD_ADJ_CAL 3
#define TURN90 160
#define TURN180 320
#define PAN_RATIO 7 / 3

int PAN_SPEED = 20;
int curPan = 0;

// light sensors
#define _SENSOR_LEFT S2
#define _SENSOR_RIGHT S3
#define SENSOR_LEFT SENSOR_2
#define SENSOR_RIGHT SENSOR_3

#define THRESHOLD 35 // 1-99
#define THRESHOLDRAW 350
#define SENSOR_MAX 1023

// sonar
#define _SONAR S4
#define SONAR SensorUS(_SONAR)

#define NEAR 15
#define FAR 100

// MODE
#define STOPPED 0
#define IDLE 1
#define DESTINATION 2
#define LINE_FOLLOW 3
#define INTERSECTION 4
#define TURNLEFT 5
#define TURNRIGHT 6
#define TURNAROUND 7
#define SONARBLOCK 8
#define CALIBRATING

int MODE = 0;

// heading
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// destination
#define STRAIGHT 0
#define LEFT 1
#define RIGHT 2
#define ARRIVED 3

// Calibration
int LEFT_MAX = 0;
int LEFT_MIN = SENSOR_MAX;
int RIGHT_MAX = 0;
int RIGHT_MIN = SENSOR_MAX;

// other
#define FOREVER while(1)

int curX;
int curY;
int heading;
int STATUS = 0;
int CALIBRATED = 0;

void setStatus(int s)
{
 STATUS |= s;
}

void remStatus(int s)
{
 STATUS &= !s;
}

int battery()
{
 return BatteryLevel() / 90;
}

void motorPower(int left, int right)
{
 OnFwd(MOTOR_LEFT,left);
 OnFwd(MOTOR_RIGHT,right);
}

void stopMotors()
{
 Off(MOTOR_LEFT);
 Off(MOTOR_RIGHT);
}

void stopWheels()
{
 motorPower(0,0);
}

void forward()
{
 motorPower(SPEED,SPEED);
}

void forwardSync(int turnpct)
{
 OnFwdSync(MOTOR_BOTH,SPEED,turnpct);
}

void forwardWait(int ms)
{
 forward();
 Wait(ms);
 stopMotors();
}

void turnRight()
{
 ResetRotationCount(MOTOR_LEFT);
 motorPower(SPEED,-SPEED);
 while(MotorRotationCount(MOTOR_LEFT)<TURN90);
 stopMotors();
 heading = (heading + 1) % 4;
}

void turnRightWait(int before, int after)
{
 forwardWait(before);
 turnRight();
 forwardWait(after);
}

void turnLeft()
{
 ResetRotationCount(MOTOR_RIGHT);
 motorPower(-SPEED,SPEED);
 while(MotorRotationCount(MOTOR_RIGHT)<TURN90);
 stopMotors();
 heading = (heading) ? (heading - 1) : 3;
}

void turnLeftWait(int before, int after)
{
 forwardWait(before);
 turnLeft();
 forwardWait(after);
}

void turnLeftRotate()
{
 RotateMotorEx(MOTOR_BOTH,SPEED,180,100,true,true);
}

void turnAround()
{
 ResetRotationCount(MOTOR_LEFT);
 motorPower(SPEED,-SPEED);
 while(MotorRotationCount(MOTOR_LEFT)<TURN180);
 stopMotors();
 heading = (heading + 2) % 4;
}

void pan(int deg)
{
 RotateMotor(MOTOR_CAMERA,PAN_SPEED,deg*PAN_RATIO);
}

int sensorLeft()
{
 int value = SENSOR_LEFT - LEFT_MIN;
 int range = LEFT_MAX - LEFT_MIN;
 return ( value*100 / range );
}

int sensorRight()
{
 int value = SENSOR_RIGHT - RIGHT_MIN;
 int range = RIGHT_MAX - RIGHT_MIN;
 return ( value*100 / range);
}

int maxSensor()
{
 int left = sensorLeft();
 int right = sensorRight();
 return (left>right) ? left : right;
}

void checkSonar()
{
 if( SONAR <= NEAR )
 {
  int last = MODE;
  MODE = SONAR;
  stopWheels();
  while( SONAR <= NEAR );
  MODE = last;
 }
}

void adjustLinePosition()
{
 int pos = sensorRight() - sensorLeft();
 int adj = pos * SPD_ADJ_CAL;
 motorPower( SPEED - adj, SPEED + adj );
}

void adjustLinePositionRaw()
{
 int pos = SENSOR_RIGHT - SENSOR_LEFT;
 pos *= 2/3;
 int adj = pos*pos;
 adj = Sqrt(adj);
 adj = Sqrt(adj);
 adj = (pos>0)?adj:-adj;
 adj = pos / SPEED_ADJUST;
 motorPower( SPEED + adj, SPEED - adj );
}

void adjustLineSyncRaw()
{
 int pos = SENSOR_RIGHT - SENSOR_LEFT;
 //int adj = pos / SPEED_ADJUST;
 pos *= 2/3;
 int adj = pos*pos;
 adj = Sqrt(adj);
 adj = Sqrt(adj);
 adj = (pos>0)?adj:-adj;
 forwardSync(adj);
}

bool aboveThreshold()
{
 if( sensorLeft() > THRESHOLD || sensorRight() > THRESHOLD )
     return true;
 return false;
}

bool aboveThresholdRaw()
{
 if( SENSOR_LEFT > THRESHOLDRAW || SENSOR_RIGHT > THRESHOLDRAW )
     return true;
 return false;
}

void setLightOn()
{
 SetSensorType(_SENSOR_LEFT, SENSOR_TYPE_LIGHT_ACTIVE);
 SetSensorType(_SENSOR_RIGHT, IN_TYPE_LIGHT_ACTIVE);
}

void setLightOff()
{
 SetSensorType(_SENSOR_LEFT, SENSOR_TYPE_LIGHT_INACTIVE);
 SetSensorType(_SENSOR_RIGHT, IN_TYPE_LIGHT_INACTIVE);
}

void setLightRaw()
{
 SetSensorMode(_SENSOR_LEFT, SENSOR_MODE_RAW);
 SetSensorMode(_SENSOR_RIGHT, SENSOR_MODE_RAW);
}

void setSonarRaw()
{
 SetSensorLowspeed(_SONAR);
}

/*
void calibrate()
{
}
*/
