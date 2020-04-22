// Ultrasonic Sensor Code
// Author: Ryan Pritchard
// Last Modified: 22/04/2020
// Description:
/* Calls functions from the NewPing library to read distances from the ultrasonic sensor
 * In it's own file to make potential changes/increases to the code easier later on.
 */

int readPing(){
  int cm = 0;
  cm = sonar.ping_cm();
  
  return cm;
}
