//Servo Movement Code
int lookRight(){
  int distance = 0;
  servo_motor.write(ServoRight); 
  delay(500);
  distance = readPing();
  delay(100);
  servo_motor.write(ServoStraight);
  
  return distance;
}

int lookLeft(){
  int distance = 0;
  servo_motor.write(ServoLeft);
  delay(500);
  distance = readPing();
  delay(100);
  servo_motor.write(ServoStraight);
  
  return distance;
}

int lookStraight(){
  int distance = 0;
  servo_motor.write(ServoStraight);
  delay(500);
  distance = readPing();
  delay(100);
  
  return distance;
}
