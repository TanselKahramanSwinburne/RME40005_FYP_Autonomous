// Laser Sensor Code
// Author: Tansel A Kahraman
// Last Modified: 14/05/2020
// Description:
/*
 * 
 */
#include <SoftwareSerial.h>

#define BAUD_RATE_LIDAR 115200
#define BAUD_RATE_SERIAL 115200
#define MOTOR 3

SoftwareSerial mySerial(10,11);

float minDistance = 100000;
float angleAtMinDist = 0;
void setup() {
  // Open serial connections (Software serial used for debugging).
  Serial.begin(BAUD_RATE_SERIAL);
  mySerial.begin(BAUD_RATE_LIDAR);

  // Setup pin for motor PWM control.
  pinMode(MOTOR, OUTPUT);

  // Connect LIDAR to software serial (note that header files needed to be changed).
  lidar.begin(mySerial);
  lidar.startScan();
  analogWrite(MOTOR, 0);
  delay(1000);
}

void loop() {
  if (IS_OK(lidar.waitPoint())) {
    // Perform data processing.
    float distance = lidar.getCurrentPoint().distance / 1000;
    float angle = lidar.getCurrentPoint().angle;
    bool startBit = lidar.getCurrentPoint().startBit;
    byte quality = lidar.getCurrentPoint().quality;

  
    // Display values to Software Serial.
    if (mySerial.available()) {
     Serial.println((String)angle + "," + (String)distance);
    }
  } else {
    analogWrite(MOTOR, 255);
    
    // try to detect RPLIDAR... 
    rplidar_response_device_info_t info;
    if (IS_OK(lidar.getDeviceInfo(info, 100))) {
       //detected...
       lidar.startScan();
       analogWrite(MOTOR, 0);
       delay(1000);
    }
  }
}
  
  
