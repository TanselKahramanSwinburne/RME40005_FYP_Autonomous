//Ultrasonic Code Read Distance
int readPing(){
  int cm = 0;
  delay(70); //If this delay is here, can get rid of all delays before and after calls to function? need tuning
  cm = sonar.ping_cm();
  
  return cm;
}
