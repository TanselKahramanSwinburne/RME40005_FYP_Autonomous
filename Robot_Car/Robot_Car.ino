#include <Servo.h>
#include <NewPing.h>

//Setup Motor Control Pins
const int LeftMotorForward = 5;
const int LeftMotorBackward = 4;
const int RightMotorForward = 3;
const int RightMotorBackward = 2;

//Servo Motor Pin
const int ServoPin = 11;

//Servo Movement Calibrations
const int ServoLeft = 190;
const int ServoRight = 0; 
const int ServoStraight = 100;

//Sensor Pins
#define TRIG_PIN A1 //Analog Input 1
#define ECHO_PIN A2 //Analog Input 2

//Movement Specifications
const int MaximumDistance = 200;
const int MinimumDistance = 20;
const int TurnTime = 500;
bool MovingForward = false;
int Distance = 100;

NewPing sonar(TRIG_PIN, ECHO_PIN, MaximumDistance); //Ultrasonic Sensor Class Creation - Note: Can potentially use the same pins for trigger and echo if running short on pins, consult NewPing.h
Servo servo_motor; //Servo Class

void setup() {
  //Motor Control Setup
  pinMode(RightMotorForward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);

  //Servo Setup
  servo_motor.attach(ServoPin);

  servo_motor.write(ServoStraight); //Initialise servo to point straight ahead
  
  delay(2000);

  //Ultrasonic Initial Readings
  Distance = readPing();
  delay(100);
  Distance = readPing();
  delay(100);
  Distance = readPing();
  delay(100);
  Distance = readPing();
  delay(100);
}

void loop() {

  int distanceRight = 0;
  int distanceLeft = 0;
  
  delay(50); //tuning - can we get rid of it?

  if(Distance <= MinimumDistance){
    moveStop();    
    delay(400);

    //Check either direction
    distanceRight = lookRight();
    distanceLeft = lookLeft();

    //Check if distanceLeft is closer than current Distance
    if (distanceRight >= distanceLeft){
      turnRight();
    }
    else {
      turnLeft();
    }
  }
  //Not close to anything, move forwards
  else {
    moveForward();
  }

  //Check Distance
  Distance = readPing();
  
}

//Servo Movement Code - can all be built to own library?
int lookRight(){
  int distance = 0;
  servo_motor.write(ServoRight); 
  delay(500);
  distance = readPing();
  delay(100);
  servo_motor.write(ServoStraight);
  return distance;
}

int lookLeft(){
  int distance = 0;
  servo_motor.write(ServoLeft);
  delay(500);
  distance = readPing();
  delay(100);
  servo_motor.write(ServoStraight);
  return distance;
}

//Ultrasonic Code - can be put in own library?
int readPing(){
  int cm = 0;
  delay(70); //If this delay is here, can get rid of all delays before and after calls to function?
  cm = sonar.ping_cm();
  if(cm==0) {
    cm = 250; //what?
  }
  return cm;
}

//Movement code - build into own library?
void moveForward(){
  if(!MovingForward) {
    MovingForward = true; //No point in having this at this time?
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);

    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
  }
}

void moveStop(){
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

void moveBackward(){
    MovingForward = false;
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(RightMotorForward, LOW);

    digitalWrite(LeftMotorBackward, HIGH);
    digitalWrite(RightMotorBackward, HIGH);
}

void turnRight(){
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);

  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);

  delay(TurnTime);

  moveStop();
}

void turnLeft(){
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);

  delay(TurnTime);

  moveStop();
}
