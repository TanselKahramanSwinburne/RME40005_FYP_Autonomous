// Movement Control Code
// Author: Ryan Pritchard
// Last Modified: 22/04/2020
// Description:
/* Sets pin values to for basic movement commands.
 * Based on the direction of travel required, sets pins high and low to control robot motors.
 */

void moveStop(){
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}
 
void moveForward(){
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);

    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
}

void moveBackward(){
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(RightMotorForward, LOW);

    digitalWrite(LeftMotorBackward, HIGH);
    digitalWrite(RightMotorBackward, HIGH);
}

void turnRight(){
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);

  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);

  delay(TurnTime);

  moveStop();
}

void turnLeft(){
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);

  delay(TurnTime);

  moveStop();
}
