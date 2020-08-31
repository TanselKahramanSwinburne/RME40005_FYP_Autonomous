void PathSelection(){
  switch (StepSequence) {
    case 1:
      step1_Running();
      break;
    case 2:
      step2_FindGap();
      break;
    case 3:
      step3_CalculateToGap();
      break;
    case 4:
      step4_RotateToGap();
      break;
    case 5:
      step5_RotateToFindGap();
      break;
    default:
      step0_Reset();
      break;
  }
}

void step0_Reset() {
  moveStop();
  
  StepSequence = 1;
  Serial.println("Step 0: Reset -> Step 1: Running");
}

void step1_Running() {
  moveForward(Slow);
  
  // Look for WORST CASE scenario in front zone.
    //tuned values for 
    if (Data_Array[1][0] <= 175 || Data_Array[1][1] <= 225 || Data_Array[1][8] <= 225 || Data_Array[1][9] <= 175){ 
      StepSequence = 2;
      Serial.println("Step 1: Running -> Step 2: Find Gap");
    }
    else{
      for(int i = 2; i < 8; i++){
         if(Data_Array[1][i] <= MinDist){ //250
          StepSequence = 2;
          Serial.println("Step 1: Running -> Step 2: Find Gap");
          break;
         }
      }
    }
}

void step2_FindGap() {
  leftGapIndex = -1;
  rightGapIndex = -1;
  
  moveStop();

  // Look for a three section 300mm gap in the left and right zones.
  for (int i = 0; i < (SECTIONS - 4); i++) {
    if (((Data_Array[2][i] >= GapDist) &&  (Data_Array[2][i + 1] >= GapDist) && (Data_Array[2][i + 2] >= GapDist) && (Data_Array[2][i + 3] >= GapDist) && (Data_Array[2][i + 4] >= GapDist)) && (rightGapIndex == -1)) {
      rightGapIndex = i + 2;
    }

    if (((Data_Array[0][SECTIONS - 1 - i] >= GapDist) &&  (Data_Array[0][SECTIONS - 1 - i - 1] >= GapDist) && (Data_Array[0][SECTIONS - 1 - i - 2 ] >= GapDist) &&  (Data_Array[0][SECTIONS - 1 - i - 3] >= GapDist) && (Data_Array[2][SECTIONS - 1 - i - 4] >= GapDist)) && (leftGapIndex == -1)) {
      leftGapIndex = SECTIONS - 1 - i - 2;
      //Serial.println("Index " + String(leftGapIndex)); 
    }
  }

  /*for(int i = SECTIONS - 1; i >= 2; i--){
    if (((Data_Array[0][i] >= 500) &&  (Data_Array[0][i - 1] >= 500) && (Data_Array[0][i - 2] >= 500)) && (leftGapIndex == -1)) {
      leftGapIndex = i - 1;
      Serial.println("Index " + String(leftGapIndex)); 
    }
  }*/
  
  if ((leftGapIndex >= 0) || (rightGapIndex >= 0)) { 
    StepSequence = 3;
    Serial.println("Step 2: Find Gap -> Step 3: Calculate to Gap");
  } else { // If a gap wasn't found on both sides, rotate until you find one.
    StepSequence = 5;
    Serial.println("Step 2: Find Gap -> Step 5: Rotate to Find Gap");
  }

}


void step3_CalculateToGap() {
  int frontAngle, frontAngleRange1, frontAngleRange2;
  int leftLocalRange1, leftLocalRange2, leftTargetAngleLocal; 
  int leftTravelAngle = 999;
  int rightLocalRange1, rightLocalRange2, rightTargetAngleLocal;
  int rightTravelAngle = 999;

  
  // We first need to determine what the angle is in front of the robot.
  frontAngleRange1 = Zone_Spacing + ((SECTIONS / 2) * Section_Spacing) + START_ANGLE;
  frontAngleRange2 = Zone_Spacing + (((SECTIONS / 2) + 1) * Section_Spacing) - 1 + START_ANGLE;
  frontAngle = (frontAngleRange1 + frontAngleRange2) / 2;
  
  // If there was a gap on the left, find the angle it is at.
  if (leftGapIndex != -1) {
    // Target angle will be the middle of the section where the gap is.
    leftLocalRange1 = ((leftGapIndex * Section_Spacing) + START_ANGLE);
    leftLocalRange2 = (((leftGapIndex + 1) * Section_Spacing) - 1 + START_ANGLE);
    leftTargetAngleLocal = (leftLocalRange1 + leftLocalRange2) / 2;

    // We must now get the angle between the front of our robot to the target angle so we can tell it how much to rotate by.
    leftTravelAngle = leftTargetAngleLocal - frontAngle;    
  }

  // If there was a gap on the right, find the angle it is at.
  if (rightGapIndex != -1) {
    // Target angle will be the middle of the section where the gap is.
    rightLocalRange1 = ((2 * Zone_Spacing) + (rightGapIndex * Section_Spacing) + START_ANGLE);
    rightLocalRange2 = ((2 * Zone_Spacing) + ((rightGapIndex + 1) * Section_Spacing) - 1 + START_ANGLE);
    rightTargetAngleLocal = (rightLocalRange1 + rightLocalRange2) / 2;

    // We must now get the angle between the front of our robot to the target angle so we can tell it how much to rotate by.
    rightTravelAngle = rightTargetAngleLocal - frontAngle;    
  }

  if (abs(leftTravelAngle) < abs(rightTravelAngle)) {
    travelAngle = leftTravelAngle;
  } else {
    travelAngle = rightTravelAngle;
  }

//    Serial.println("frontAngle: " + String(frontAngle));
//    
//    Serial.print("leftTargetAngleLocal: " + String(leftTargetAngleLocal));
//    Serial.println(" | leftTravelAngle: " + String(leftTravelAngle));
//    
//    Serial.print("rightTargetAngleLocal: " + String(rightTargetAngleLocal));
//    Serial.println(" | rightTravelAngle: " + String(rightTravelAngle));
//
//    Serial.println("travelAngle: " + String(travelAngle));

    // [ROBOT ROTATE UNTIL COMPASS GOES TO THIS BEARING]
   // StepSequence = 4;
    Serial.println("Step 3: Calculate to Gap -> Step 4: Rotate to Gap");

  
  Heading = readCompass() + travelAngle;
  if(Heading >= 360){
        Heading -= 360;
      }
  if(Heading < 0){
    Heading += 360;
  }
  StepSequence = 4;
}


void step4_RotateToGap() {
  // [ROBOT ROTATE UNTIL COMPASS BEARING CHANGES EQUIVALENT TO travelAngle]
  int bearing = -1;
  bearing = readCompass();
  Serial.println("Bearing: " + String(bearing));
  Serial.println("Heading: " + String(Heading));
  if(Heading != bearing && Heading != -1){
    if(Heading > bearing + 10){
      turnLeft(Med);
    } else if(Heading < bearing - 10){
      turnRight(Med);
    } else if(Heading < bearing + 10 && Heading > bearing - 10){
      StepSequence = 1;
      Serial.println("Step 4: Rotate to Gap -> Step 1: Running");
    }
  }
}

void step5_RotateToFindGap() {
  // [ROBOT ROTATE UNTIL GAP FOUND]
  StepSequence = 1;
}
