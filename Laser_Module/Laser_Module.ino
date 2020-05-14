// Laser Sensor Code
// Author: Tansel A Kahraman
// Last Modified: 14/05/2020
// Description:
/*
 * 
 */
#include <RPLidar.h>
#include <SoftwareSerial.h>

#define BAUD_RATE 9600
#define MOTOR 3

SoftwareSerial mySerial(10,11);
RPLidar lidar;

float minDistance = 100000;
float angleAtMinDist = 0;
void setup() {
  // Open serial connections (Software serial used for debugging).
  Serial.begin(BAUD_RATE);
  mySerial.begin(BAUD_RATE);

  // Setup pin for motor PWM control.
  pinMode(MOTOR, OUTPUT);

  // Connect LIDAR to software serial (note that header files needed to be changed).
  lidar.begin(mySerial);
}

void loop() {
  if (IS_OK(lidar.waitPoint())) {
    // Perform data processing.
    float distance = lidar.getCurrentPoint().distance;
    float angle = lidar.getCurrentPoint().angle;
    bool startBit = lidar.getCurrentPoint().startBit;
    byte quality = lidar.getCurrentPoint().quality;

  
    // Display values to Software Serial.
    if (mySerial.available()) {
     Serial.println((String)angle + "," + (String)distance);
    }
  } else {
    analogWrite(MOTOR, 0);
    
    // try to detect RPLIDAR... 
    rplidar_response_device_info_t info;
    if (IS_OK(lidar.getDeviceInfo(info, 100))) {
       //detected...
       lidar.startScan();
       analogWrite(MOTOR, 255);
       delay(1000);
    }
  }
}
  
  
