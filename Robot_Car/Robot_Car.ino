// Robot Car Main Code
// Author: Ryan Pritchard
//         Tansel Kahraman
// Last Modified: 03/05/2020
// Description:
/* Main setup and control code for robot car.
 * Sets up controller pin designation, global variables, ultrasonic sensor, compass module, Laser Module and servo motor
 * contains high level control code for the robot
 * Note: Other .ino files, Movement, Servo_Control and Ultrasonic are all merged into this file at compile time, they are added in alphabetical order.
 *       Be careful with order of declared functions to ensure that the using function is declared in a file that is merged after the function it is using.
 * Note: Pins 10 and 9 do not seem to work as PWM pins on this arduino UNO
 */
 //Temp Note: turn time delay and movestop are commented out in the turn left function at the minute during testing of the compass and PWM
 //           Likely remove the delays inside the turn functions so that turning can be controlled by the compass (PID?)
 //           Thinking not to worry about accurate turning too much just yet, likely will be able to use the wheelchair in semester 2 so focus on path planning
 
 
#include <Servo.h> //required for servo
#include<math.h> //required for compass
#include <HardwareSerial.h> //required for laser sensor - Serial buffer has been extended in this library to 256 bytes instead of original 64 as overflows were occurring
#include "NewPing.h" //required for ultrasonics
#include "QMC5883LCompass.h" //required for compass

//Laser Comms Defines
#define BAUD_RATE 115200 //laser baud rate
#define MOTOR 12
#define PRINT_TIME 500
#define INITIAL_BYTE 170
#define SECONDARY_BYTE 85
#define RUN_MESSAGE 0
#define ARRAY_SIZE 100 //10 bytes of header + 80 bytes of data (2 bytes per sample) + 10 spare bytes to avoid errors*/  

//Data Configuration Defines
#define ZONES 3
#define SECTIONS 10
#define START_ANGLE 0
#define END_ANGLE 270
#define MID_ANGLE 135//need to calibrate

//Path Selection Variables
int StepSequence;
int leftGapIndex = -1;
int rightGapIndex = -1;
int travelAngle;
int Heading = -1;
const int GapDist = 275;
const int MinDist = 250;


//Laser Comms Variables
int AngleData[360];

typedef enum { FIRST_BYTE, SECOND_BYTE, TYPE, SAMPLE_NUM, GET_DATA, NONE } states;
states state = NONE;

//Data Configuration Variables
int Data_Array[ZONES][SECTIONS];
int Zone_Spacing;
int Section_Spacing;
String zRange1, zRange2, sRange1, sRange2;

//testing turning code
int initial = 0;
int tempCounter = 0;
int turnCounter = 0;

//Printing time count values
float SavedTime = 0;
float CurrentTime = 0;

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
bool AtBearing = false;

//Ultrasonic Sensor Pins
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
  Serial.begin(BAUD_RATE);
  Serial1.begin(BAUD_RATE);  
  
  SetupLaserComms();
  SetupDataConfig();
  setupCompass();

  //path selection setup
  StepSequence = 0;
  
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



void PrintStuff(){
  for (int i = 0; i < ZONES; i++) {
    for (int j = 0; j < SECTIONS; j++) {
      
      Serial.print("["+String(Data_Array[i][j]) + "]");
    }
    Serial.println();
  }
  
  Serial.println("---------------------------------------------------------------------------------------------");
 Serial.println(String(StepSequence));
}


void loop() {

  int distanceRight = 0; 
  int distanceLeft = 0;

  int degreesRead = 0; //Compass current degree value
  
  
  //Read Messages from Lidar
  while(Serial1.available() > 0){
    CheckMessage(Serial1.read());
  }
  UpdateDataArray();

  //Print data array
  if(SavedTime == 0){
    SavedTime = millis();
  }
  CurrentTime = millis() - SavedTime;

  if(CurrentTime > 2000 && CurrentTime < 2500){
    PrintStuff();
    SavedTime = millis();
  }
  if(millis() > 5000){
    PathSelection();  
  }
  
  
  
  
//  Read Compass Value
//  degreesRead = readCompass();
//  Serial.print("Deg: ");
//  Serial.print(degreesRead);
//  Serial.println();

  //turn until bearing

 /*
  if(bearing - 9 > degreesRead){ //offset of about 9° for turning control
    turnLeft(Slow); //two types of turns required, wheels backwards and not?
    initial = 0;
  }
  //else if (bearing > degreesRead){
    //bumpLeft(Med);
  //}
  else if (initial == 0){
    //brake(Med);
    initial = 1;
    //delay(BrakeTime);
  }
  else {
    moveStop();
  }
  
   */ 
    //PWM Test going straight
    //Right motor stronger than left by about 10
    //90 is about the lowest speed we can go, 90 on left, 80 on right, might be different for one forwards one backwards
    
 
  
  
}
