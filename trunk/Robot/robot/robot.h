// mailboxes
#define OUTBOX 0  // to controller
#define INBOX 1   // from controller

// motors
#define MOTOR_LEFT OUT_A
#define MOTOR_RIGHT OUT_C
#define MOTOR_BOTH OUT_AC
#define MOTOR_CAMERA OUT_B

#define SPEED 30
#define SPEED_ADJUST 4
#define SPD_ADJ_CAL 3
#define SPEED_CONTROL 15
#define TURN90 160
#define TURN180 320
#define PAN_RATIO 7 / 3
#define PGAIN 50
#define IGAIN 50
#define DGAIN 50

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
#define IDLE 1                //1
#define DESTINATION 2         //2
#define LINE_FOLLOW 3         //4
#define INTERSECTION 4        //8
#define TURNLEFT 5            //16
#define TURNRIGHT 6           //32
#define TURNAROUND 7          //64
#define SONARBLOCK 8          //128
#define CALIBRATING 9         //256
#define PAN 10                //512
#define STOPPED 11            //1024

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

void remStatusIDLE()
{
 STATUS &= -1-(1<<IDLE);
}

void setStatus(int s)
{
 STATUS |= (1 << s);

 remStatusIDLE();
}

void remStatus(int s)
{
 STATUS &= -1-(1 << s);
 
 if( ( STATUS == 0 )
  || ( STATUS == (1<<INTERSECTION) )
  || ( STATUS != (1<<PAN) ) )
     setStatus(IDLE);
}

bool checkStatus(int s)
{
 return ( STATUS & (1<<s) ) > 0;
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

void forwardWait(int ms)
{
 forward();
 Wait(ms);
 stopMotors();
}

void turnRight()
{
 setStatus(TURNRIGHT);
 
 ResetRotationCount(MOTOR_LEFT);
 motorPower(SPEED,-SPEED);
 while(MotorRotationCount(MOTOR_LEFT)<TURN90);
 stopMotors();
 heading = (heading + 1) % 4;
 
 remStatus(TURNRIGHT);
}

void turnRightWait(int before, int after)
{
 forwardWait(before);
 turnRight();
 forwardWait(after);
}

void turnLeft()
{
 setStatus(TURNLEFT);
 
 ResetRotationCount(MOTOR_RIGHT);
 motorPower(-SPEED,SPEED);
 while(MotorRotationCount(MOTOR_RIGHT)<TURN90);
 stopMotors();
 heading = (heading) ? (heading - 1) : 3;
 
 remStatus(TURNLEFT);
}

void turnLeftWait(int before, int after)
{
 forwardWait(before);
 turnLeft();
 forwardWait(after);
}

void turnAround()
{
 setStatus(TURNAROUND);
 
 ResetRotationCount(MOTOR_LEFT);
 motorPower(SPEED,-SPEED);
 while(MotorRotationCount(MOTOR_LEFT)<TURN180);
 stopMotors();
 heading = (heading + 2) % 4;
 
 remStatus(TURNAROUND);
}

// degrees
void pan(int deg)
{
 setStatus(PAN);
 
 // normalize input
 while( deg < -360 )
        deg += 360;
 deg %= 360;
 
 curPan += deg;
 
 RotateMotor(MOTOR_CAMERA,PAN_SPEED,deg*PAN_RATIO);
 
 // normalize current
 while( curPan < 0 )
        curPan += 360;
 curPan %= 360;
 
 remStatus(PAN);
}

// angle
void pid(int angle)
{
 setStatus(PAN);
 
 // normalize input
 while( angle < -360 )
        angle += 360;
 angle %= 360;

 curPan += angle;
 
 RotateMotorPID(MOTOR_CAMERA,PAN_SPEED,angle*PAN_RATIO,PGAIN,IGAIN,DGAIN);
 
 // normalize current
 while( curPan < 0 )
        curPan += 360;
 curPan %= 360;
 
 remStatus(PAN);
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

void lineCorrect()
{
 int left, right;
  
 setLightOn();
 Wait(10);
 do
 {
    left = sensorLeft();
    right = sensorRight();

    if( left < right )
    {
     motorPower(-SPEED,SPEED);
    }
    else
    {
     motorPower(SPEED,-SPEED);
    }
 } while( abs(right-left) > 5 );
 
 stopWheels();
}

void checkSonar()
{
 if( SONAR <= NEAR )
 {
  setStatus(SONARBLOCK);
  
  int last = MODE;
  MODE = SONAR;
  
  stopWheels();
  while( SONAR <= NEAR );

  MODE = last;
  
  remStatus(SONARBLOCK);
 }
}

void adjustLinePosition()
{
 int pos = sensorRight() - sensorLeft();
 int adj = pos * SPD_ADJ_CAL;

 if( adj < -SPEED_CONTROL ) adj = -SPEED_CONTROL;
 else if( adj > SPEED_CONTROL ) adj = SPEED_CONTROL;

 motorPower( SPEED - adj, SPEED + adj);
}

bool aboveThreshold()
{
 if( sensorLeft() > THRESHOLD || sensorRight() > THRESHOLD )
     return true;
 return false;
}

void intersection()
{
 if( MODE != STOPPED ){
     setStatus(INTERSECTION);

     switch(heading)
     {
      case NORTH : curY++; break;
      case EAST  : curX++; break;
      case SOUTH : curY--; break;
      case WEST  : curX--; break;
     }
 }
}

void lineFollow()
{
 remStatus(STOPPED);
 remStatus(INTERSECTION);
 setStatus(LINE_FOLLOW);
 
 setLightOn();

 Wait(100);

 do {
   checkSonar();  // blocking
   adjustLinePosition();
 } while( aboveThreshold() && checkStatus(STOPPED) );

 stopWheels();
 
 intersection();

 stopWheels();
 remStatus(LINE_FOLLOW);
}

void calibrate()
{
 setStatus(CALIBRATING);
 
 int left, right;

 setLightOn();

 motorPower(25,-25);
 repeat(10)
 {
  Wait(100);
  left = SENSOR_LEFT;
  right = SENSOR_RIGHT;

  if(left<LEFT_MIN) LEFT_MIN = left;
  else if(left>LEFT_MAX) LEFT_MAX = left;

  if(right<RIGHT_MIN) RIGHT_MIN = right;
  else if(right>RIGHT_MAX) RIGHT_MAX = right;
 }
 stopWheels();
 motorPower(-25,25);
 repeat(20)
 {
  Wait(100);
  left = SENSOR_LEFT;
  right = SENSOR_RIGHT;

  if(left<LEFT_MIN) LEFT_MIN = left;
  else if(left>LEFT_MAX) LEFT_MAX = left;

  if(right<RIGHT_MIN) RIGHT_MIN = right;
  else if(right>RIGHT_MAX) RIGHT_MAX = right;
 }
 stopWheels();
 motorPower(25,-25);
 repeat(10)
 {
  Wait(100);
  left = SENSOR_LEFT;
  right = SENSOR_RIGHT;

  if(left<LEFT_MIN) LEFT_MIN = left;
  else if(left>LEFT_MAX) LEFT_MAX = left;

  if(right<RIGHT_MIN) RIGHT_MIN = right;
  else if(right>RIGHT_MAX) RIGHT_MAX = right;
 }
 stopWheels();

 CALIBRATED = true;
 
 remStatus(CALIBRATING);
}
