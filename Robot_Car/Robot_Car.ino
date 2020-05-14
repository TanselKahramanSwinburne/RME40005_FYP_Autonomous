// Robot Car Main Code
// Author: Ryan Pritchard
//         Tansel Kahraman
// Last Modified: 03/05/2020
// Description:
/* Main setup and control code for robot car.
 * Sets up controller pin designation, global variables, ultrasonic sensor, compass module and servo motor
 * contains high level control code for the robot
 * Note: Other .ino files, Movement, Servo_Control adn Ultrasonic are all merged into this file at compile time, they are added in alphabetical order.
 *       Be careful with order of declared functions to ensure that the using function is declared in a file that is merged after the function it is using.
 * Note: Pins 10 and 9 do not seem to work as PWM pins on this arduino UNO
 */
 //Temp Note: turn time delay and movestop are commented out in the turn left function at the minute during testing of the compass and PWM
 
#include <Servo.h> //required for servo
#include<math.h> //required for compass
#include "NewPing.h" //required for ultrasonics
#include "QMC5883LCompass.h" //required for compass

 int initial = 0;
int tempCounter = 0;
int turnCounter = 0;

//Setup Compass Constants and Variables
const int CompArraySize = 1;
const int CompSmoothing = 8;
int AveragingXArray[CompArraySize];
int AveragingYArray[CompArraySize];
int AveragingLoop = 0;

//Setup Motor Control Pins
const int LeftMotorForward = 5;
const int LeftMotorBackward = 4;
const int RightMotorForward = 3;
const int RightMotorBackward = 2;
const int LeftMotorPWM = 11;
const int RightMotorPWM = 6;

//Servo Motor Pin
const int ServoPin = 10;

//Servo Movement Calibrations
const int ServoLeft = 190;
const int ServoRight = 0; 
const int ServoStraight = 100;

//Sensor Pins
#define TRIG_PIN A1 //Analog Input 1
#define ECHO_PIN A2 //Analog Input 2

//Movement Specifications
const int Fast = 255; //Motor PWM Speed Settings
const int Med = 180;
const int Slow = 90;
const int Off = 0;
const int MaximumDistance = 200;
const int MinimumDistance = 20;
const int TurnTime = 10; //Min required 10ms, 300 is roughly 90 degrees - roughly. it changes how far it goes each time.
const int BumpTime = 50; //Essentially bump time is what turn time does at the minute, should adjust turn time for a set angle?
const int BrakeTime = 50;
bool MovingForward = false;
int Distance = 100;


//Class Declarations
NewPing sonar(TRIG_PIN, ECHO_PIN, MaximumDistance); //Ultrasonic Sensor Class Creation - Note: Can potentially use the same pins for trigger and echo if running short on pins, consult NewPing.h
Servo servo_motor; //Servo Class
QMC5883LCompass compass; //Compass class declaration

void setup() {
  //Compass Setup
  Serial.begin(9600);
  setupCompass();
  
  //Motor Control Setup
  pinMode(RightMotorForward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(LeftMotorPWM, OUTPUT);
  pinMode(RightMotorPWM, OUTPUT);

  //Servo Setup
  servo_motor.attach(ServoPin);
  servo_motor.write(ServoStraight); //Initialise servo to point straight ahead
}

void loop() {

  int distanceRight = 0; 
  int distanceLeft = 0;

  int degreesRead = 0; //Compass current degree value

  

  //Read Compass Value
  degreesRead = readCompass();
  Serial.print("Deg: ");
  Serial.print(degreesRead);
  Serial.println();

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
  //turn until bearing
  int bearing = 250;
 
  if(bearing - 9 > degreesRead){ //offset of about 9° for turning control
    turnLeft(Slow); //two types of turns required, wheels backwards and not?
    initial = 0;
  }
  /*else if (bearing > degreesRead){
    bumpLeft(Med);
  }*/
  else if (initial == 0){
    brake(Med);
    initial = 1;
    //delay(BrakeTime);
  }
  else {
    moveStop();
  }
  //  Serial.print(initial);
  //  Serial.println();
     //add in braking to movestop?
    //include a bump command to bump left 2 or 3 degrees, closest seems to get is about 4° before the target, bump 4 degrees or use one left one right to move the extra 4?
    //PWM Test going straight
    //analogWrite(PIN, DUTY CYCLE) - writes a PWM signal to the specified pin, at a duty cycle of 0-255 for 0-100%
    //100 and below the motors do not turn
    
    
    //Right motor stronger than left by about 10
    //90 is about the lowest speed we can go, 90 on left, 80 on right, might be different for one forwards one backwards
    
  /*if(tempCounter >= 100){
    turnLeft();
    tempCounter = 0;
    turnCounter++;
  }
    tempCounter++;
    /*
  Serial.print("Counter: ");
  Serial.print(tempCounter);
  Serial.println();
  Serial.print("Turns: ");
  Serial.print(turnCounter);
  Serial.println();*/
  
  
}
