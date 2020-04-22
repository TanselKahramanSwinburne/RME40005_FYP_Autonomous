// Robot Car Main Code
// Author: Ryan Pritchard
//         Tansel Kahraman
// Last Modified: 22/04/2020
// Description:
/* Main setup and control code for robot car.
 * Sets up controller pin designation, global variables, ultrasonic sensor and servo motor
 * contains high level control code for the robot
 * Note: Other .ino files, Movement, Servo_Control adn Ultrasonic are all merged into this file at compile time, they are added in alphabetical order.
 *       Be careful with order of declared functions to ensure that the using function is declared in a file that is merged after the function it is using.
 */
 
#include <Servo.h>
#include "NewPing.h"


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
const int TurnTime = 300; //Min required 10ms, 300 is roughly 90 degrees - roughly. it changes how far it goes each time.
bool MovingForward = false;
int Distance = 100;

//Class Declarations
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
}

void loop() {

  int distanceRight = 0;
  int distanceLeft = 0;

  //can get rid of this later, left here to have something to play with if we wanted to test hardware for now
  /*
    //Check Distance
    Distance = readPing();
   
    if(Distance <= MinimumDistance){
    moveStop();    
    delay(500);

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

  */
  turnLeft();
  delay(5000);
  
}
