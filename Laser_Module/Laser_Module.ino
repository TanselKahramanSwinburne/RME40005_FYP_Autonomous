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

void setup() {
  // Open serial connections (Software serial used for debugging).
  Serial.begin(BAUD_RATE_SERIAL);
  mySerial.begin(BAUD_RATE_LIDAR);

  // Setup pin for motor PWM control.
  pinMode(MOTOR, OUTPUT);
  analogWrite(MOTOR, 0);

}

void loop() {
    if (mySerial.available()) {
     Serial.println();
    }
}
  
  
