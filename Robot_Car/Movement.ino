// Movement Control Code
// Author: Ryan Pritchard
// Last Modified: 22/04/2020
// Description:
/* Sets pin values to for basic movement commands.
   Based on the direction of travel required, sets pins high and low to control robot motors.
   Speed is set by passing in a value to write to the PWM pins for the motor controller.
   have preset speeds of Fast, Med and Slow and Off for the moment at 255, 180, 100 out of a max of 255.
   Commands: Stop Motors, Brake going forwards, backwards, left and right, turn hard left and right, turn softly left and right,
   move forwards and backwards, bump left and right.
*/

void moveStop() { //add in braking?
  analogWrite(LeftMotorPWM, Off);
  analogWrite(RightMotorPWM, Off);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);

  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

void brakeForward(int pace){
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  
  delay(BrakeTime);

  //moveStop();
}

void brakeBackward(int pace){
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorForward, HIGH);

  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  
  delay(BrakeTime);

  //moveStop();
}

void brakeLeft(int pace){
  analogWrite(LeftMotorPWM, Off);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  
  delay(BrakeTime);

  //moveStop();
}

void brakeRight(int pace){
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, Off);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  
  delay(BrakeTime);

  //moveStop();
}

void moveForward(int pace) {
  analogWrite(LeftMotorPWM, pace + 10);
  analogWrite(RightMotorPWM, pace - 10);

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorForward, HIGH);

  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

void moveBackward(int pace) {
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
}

void turnRight(int pace) {
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, Off);

  digitalWrite(LeftMotorForward, HIGH);

  digitalWrite(RightMotorBackward, LOW);

  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);

  //delay(TurnTime);

  //moveStop();
}

void turnLeft(int pace) {
  analogWrite(LeftMotorPWM, Off);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorBackward, LOW);

  digitalWrite(RightMotorForward, HIGH);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);

  //delay(TurnTime);

  //moveStop();
}
void turnHardRight(int pace) {
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);

  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);

  //delay(TurnTime);

  //moveStop();
}

void turnHardLeft(int pace) {
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);

  //delay(TurnTime);

  //moveStop();
}
void bumpRight(int pace) {
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);

  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);

  delay(BumpTime);

  moveStop();
}

void bumpLeft(int pace) {
  analogWrite(LeftMotorPWM, pace);
  analogWrite(RightMotorPWM, pace);

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);

  delay(BumpTime);

  moveStop();
}
