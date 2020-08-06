// Laser Sensor Code
// Author: Tansel A Kahraman
// Last Modified: 19/05/2020
// Description:
/*    Basic YDLIDAR x2 code to get readings of degrees and angles.
 *    Note YDLidar class files have other writing capabilities designed for the x4. 
 *    Note that the YDLidar class needs to be changed when implemented into the robot so SoftwareSerial -> HardwareSerial.
 *    Working with 50 - 350 degrees as outside range occurs less often.
 *    Readings get transformed into sections, each section has a single reading that represents the worst case of all degrees within that section.
 *    Sections get transformed into zones, where each zone has three colours, green, yellow & red.
 *      Green = All clear.
 *      Yellow = Possible Obstacles / Obstacles in the distance
 *      Red = No Go, full obstacle, avoid.
 */
 
#include "YDLidar.h"

#define BAUD_RATE 115200
#define MOTOR 3
#define SZ_SIZE 7
#define CLEAR_TIME 5000
#define GREEN_ZONE 3000
#define YELLOW_ZONE 2000
#define RED_ZONE 500

YDLidar lidar;

char zones[SZ_SIZE];
float sections[SZ_SIZE];
 
bool isScanning = true;
float sectionIncrement = 0;
float saveTime = 0;
float clearTime;



void setup() {
  // Setup serial communications for printing and with the YLIDAR.
  Serial.begin(BAUD_RATE);
  lidar.begin(Serial, BAUD_RATE);

  // Setup motor control on the YLIDAR.
  pinMode(MOTOR, OUTPUT);
  analogWrite(MOTOR, 255);

  // Define how big a section is as per the section size.
  sectionIncrement = (360 - 0) / SZ_SIZE;

  // Initialise the sections array.
  for (int i = 0; i < SZ_SIZE; i++) {
    sections[i] = -1;
  }
}

// Function for saving data into the relevant section.
void savesection(float angle, float distance) {
  // Loop through the sections array.
  for (int i = 0; i < SZ_SIZE; i++) {
    // Check different ranges according to the section increment. 
    if ((angle >= (50 + (i * sectionIncrement))) && (angle < (50 + ((i + 1) * sectionIncrement)))) {
      // Only update the value if the array is in initialise mode (i.e. value 0), or if the current reading is less than what is stored in the array.
      if (((distance > 0) && (distance < sections[i])) || sections[i] == -1) {
        sections[i] = distance;
      }
    }
  }
}

void savezone() {
  for (int i = 0; i < SZ_SIZE; i++) {
    if ((sections[i] > GREEN_ZONE) || ((sections[i] > YELLOW_ZONE) && (sections[i] <= GREEN_ZONE))) {
      zones[i] = 'G'; 
    } else if ((sections[i] > RED_ZONE) && (sections[i] <= YELLOW_ZONE)) {
      zones[i] = 'Y';
    } else if ((sections[i] <= RED_ZONE) && (sections[i] != -1)) {
      zones[i] = 'R';
    } else if (sections[i] == -1) {
      zones[i] = 'x';
    }
  }
}


void loop() {
  // Save reading from the YLIDAR.
  if (lidar.waitScanDot() == RESULT_OK) {
    float angle    = lidar.getCurrentScanPoint().angle; //anglue value in degree

      float distance = lidar.getCurrentScanPoint().distance; //distance value in mm unit
      byte  quality  = lidar.getCurrentScanPoint().quality; //quality of the current measurement
      bool  startBit = lidar.getCurrentScanPoint().startBit;
      savesection(angle, distance);

      Serial.print("Angle: ");
      Serial.print(angle);
      Serial.print(" | Distance: ");
      Serial.println(distance);
    

     
  }

  savezone();

  // If we haven't started timing, start.
  if (saveTime == 0) {
    saveTime = millis();
  }

  // Calculate how long it has been before we clear the data.
  clearTime = millis() - saveTime;

  // If it has been longer than the defined clear time, clear the data.
  if (clearTime > CLEAR_TIME) {
    for (int i = 0; i < SZ_SIZE; i++) {
      sections[i] = -1;
    } 
    saveTime = millis();
  }

   
  /*
  for (int i = 0; i < SZ_SIZE; i++) {
    Serial.print("[");
    Serial.print(zones[i]);
    Serial.print("]");
  }
  Serial.println();*/
}


  
  
