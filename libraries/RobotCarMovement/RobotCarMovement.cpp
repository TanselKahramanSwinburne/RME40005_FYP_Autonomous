#include "RobotCarMovement.h"

//Movement code - build into own library?
void moveForward(){
  //if(!MovingForward) {
    //MovingForward = true; //No point in having this at this time?
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);

    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
  //}
}

void moveStop(){
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

void moveBackward(){
    //MovingForward = false;
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(RightMotorForward, LOW);

    digitalWrite(LeftMotorBackward, HIGH);
    digitalWrite(RightMotorBackward, HIGH);
}
