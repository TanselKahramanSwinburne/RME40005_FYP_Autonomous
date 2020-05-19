// Laser Sensor Code
// Author: Tansel A Kahraman
// Last Modified: 19/05/2020
// Description:
/*    Basic YDLIDAR x2 code to get readings of degrees and angles.
 *    Note YDLidar class files have other writing capabilities designed for the x4. 
 *    Note that the YDLidar class needs to be changed when implemented into the robot so SoftwareSerial -> HardwareSerial.
 */
#include "YDLidar.h"
#include <SoftwareSerial.h>

#define BAUD_RATE_LIDAR 115200
#define BAUD_RATE_SERIAL 115200
#define MOTOR 3

YDLidar lidar;
bool isScanning = true;
SoftwareSerial mySerial(10, 11);


void setup() {
  Serial.begin(BAUD_RATE_SERIAL);
  mySerial.begin(BAUD_RATE_LIDAR);
  lidar.begin(mySerial, BAUD_RATE_LIDAR);
  
  pinMode(MOTOR, OUTPUT);
  analogWrite(MOTOR, 0);
}

void loop() {
    if (lidar.waitScanDot() == RESULT_OK) {
      float distance = lidar.getCurrentScanPoint().distance; //distance value in mm unit
      float angle    = lidar.getCurrentScanPoint().angle; //anglue value in degree
      byte  quality  = lidar.getCurrentScanPoint().quality; //quality of the current measurement
      bool  startBit = lidar.getCurrentScanPoint().startBit;
      Serial.print(angle, DEC);
      Serial.print(",");
      Serial.println(distance, DEC);
    } else {
      //Serial.println(" YDLIDAR get Scandata fialed!!");
    }

}


  
  
