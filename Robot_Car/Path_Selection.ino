float turnTime1 = -1.0;
float turnTime2 = -1.0;
float turnTime3 = -1.0;
float totalTurnTime = -1.0;
float startTime = -1.0;

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
   case 6:
      step6_Divert_Forward1();
      break;
   case 7:
      step7_Divert_Rotate1();
      break;
   case 8:
      step8_Divert_Forward2();
      break;
   case 9:
      step9_Divert_Rotate2();
      break;
   case 10:
      step10_Divert_Forward3();
      break;
   case 11:
      step11_ReturnToPath();
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
  //moveForward(Slow);
  // Look for WORST CASE scenario in front zone.
  //tuned values for 
  step2_ClearCheck("1");
}

void step2_ClearCheck(String callingStep) {
  if (Data_Array[1][0] <= 175 || Data_Array[1][1] <= 225 || Data_Array[1][8] <= 225 || Data_Array[1][9] <= 175){ 
    StepSequence = 2;
    Serial.println("Step " + callingStep + " -> Step 2: Find Gap");
    
  }
  else{
    for(int i = 2; i < 8; i++){
       if(Data_Array[1][i] <= MinDist){ //250
        StepSequence = 2;
        
        Serial.println("Step " + callingStep + " -> Step 2: Find Gap");
        break;
       }
    }
  }
}


void step2_FindGap() {
  leftGapIndex = -1;
  rightGapIndex = -1;
  
  //moveStop();

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
  if (leftGapIndex != -1) {//7
    // Target angle will be the middle of the section where the gap is.
    leftLocalRange1 = (((leftGapIndex +1) * Section_Spacing) + START_ANGLE);//63
    leftLocalRange2 = (((leftGapIndex + 2) * Section_Spacing) - 1 + START_ANGLE);//71
    leftTargetAngleLocal = (leftLocalRange1 + leftLocalRange2) / 2;//134/2 = 67

    // We must now get the angle between the front of our robot to the target angle so we can tell it how much to rotate by.
    leftTravelAngle = leftTargetAngleLocal - frontAngle;    
  }

  // If there was a gap on the right, find the angle it is at.
  if (rightGapIndex != -1) {//2
    // Target angle will be the middle of the section where the gap is.
    rightLocalRange1 = ((2 * Zone_Spacing) + ((rightGapIndex)* Section_Spacing) + START_ANGLE); //198
    rightLocalRange2 = ((2 * Zone_Spacing) + ((rightGapIndex + 1) * Section_Spacing) - 1 + START_ANGLE); //206
    rightTargetAngleLocal = (rightLocalRange1 + rightLocalRange2) / 2;//202

    // We must now get the angle between the front of our robot to the target angle so we can tell it how much to rotate by.
    rightTravelAngle = rightTargetAngleLocal - frontAngle;    
  }

  if (abs(leftTravelAngle) < abs(rightTravelAngle)) {
    travelAngle = leftTravelAngle;
    Serial.println("Turn left for gap");
  } else {
    travelAngle = rightTravelAngle;
    Serial.println("Turn right for gap");
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
//    Serial.println("GapIndex: " + String(leftGapIndex));
//    Serial.println("RightGapIndex: " + String(rightGapIndex));
//    
    Serial.println("Step 3: Calculate to Gap -> Step 4: Rotate to Gap");
    //PrintStuff();
    StepSequence = 4;
}


void step4_RotateToGap() {
  if (startTime == -1.0) {
    startTime = millis();
  }
  
  if (travelAngle < 0) {
    // [ROBOT ROTATE LEFT]
  } else if (travelAngle > 0) {
    // [ROBOT ROTATE RIGHT]
  } else {
    // TO DO.
  } 

  // Rotate until the gap is in front of us.
  if (((Data_Array[1][3] >= GapDist) &&  (Data_Array[1][4] >= GapDist) && (Data_Array[1][5] >= GapDist) && (Data_Array[1][6] >= GapDist) && (Data_Array[1][7] >= GapDist))) {
    Serial.println("Step 4: Rotate To Gap -> Step 6: Divert Forward 1");
    StepSequence = 6;
    
  }
}

void step5_RotateToFindGap() {
  // [ROBOT ROTATE UNTIL GAP FOUND]
  StepSequence = 1;
}



void step6_Divert_Forward1() {
  // [ROBOT GOES MOVES FORWARD].
  if (turnTime1 == -1.0) {
    turnTime1 = millis() - startTime;
    startTime = -1.0;
  }
  
  // Check for any obstacles on divert path.
  //step2_ClearCheck("6");
  
  bool clearWay = true;
  if (travelAngle < 0) {
    // When we divert to left side, check if obstacle on right side is past robot.
    for (int i = 0; i =< SECTIONS / 2; i++) {
      if (Data_Array[2][i] <= GapDist) {
        clearWay = false;
      }
    }
  } else if (travelAngle > 0) {
    //Serial.println("Checking if past object");
    // When we divert to the right side, check if obstacle on left side is past robot.
    for (int i = SECTIONS / 2; i >= 0; i--) {
      if (Data_Array[0][i] >= GapDist + DistBuffer) { // POSSIBLY ADD BUFFER ON GAP DIST CHECK
        clearWay = false;
        //Serial.println("Not Past Object: " + String(GapDist) + " Section: " + String(i));
      }
    }
  } else {
    // TO DO.
  }

  if (clearWay) {
    Serial.println("Step 6: Divert Forward 1 -> Step 7: Divert Rotate 1");
    StepSequence = 7;
    
  }
}

void step7_Divert_Rotate1() {
  if (startTime == -1.0) {
    startTime = millis();
  }
  //[ROBOT ROTATE BACK ACCORDING TO INITIAL DIVERT ROTATION, might need to just have it on a timer and see how we go.]
  //[Possibly rotate until laser reads certain amounts on the side it should be rotating to?]
  
  bool rotateBack = true;
  
  if (travelAngle < 0) {  
  // If we diverted left, rotate to the right until the object is within view again.
  // [ROBOT ROTATE RIGHT]
    for (int i = SECTIONS / 2; i < SECTIONS; i++) {
      if (Data_Array[2][i]  > GapDist) {
        rotateBack = false;
      }
    }
  } else if (travelAngle > 0) {
    // If we diverted right, rotate left until the object is within view again.
    // [ROBOT ROTATE LEFT]
    for (int i = (SECTIONS / 6); i >= 0; i--) {
      if (Data_Array[0][i]  < GapDist) {
        rotateBack = false;
      }
    }
  } else {
    // TO DO.
  }

  if (rotateBack) {
    Serial.println("Step 7: Divert Rotate 1 -> Step 8: Divert Forward 2");
    StepSequence = 8;
    
  }
}

void step8_Divert_Forward2() {
    // [ROBOT GOES MOVES FORWARD].
    if (turnTime2 == -1.0) {
      turnTime2 = millis() - startTime;
      startTime = -1.0;
    }
  // Check for any obstacles on divert path.
  //step2_ClearCheck("8");
  
  bool clearWay = true;
  if (travelAngle < 0) {
    // When we divert to left side, check if obstacle on right side is past robot.
    for (int i = 0; i < (SECTIONS / 2); i++) {
      if (Data_Array[2][i] <= GapDist) {
        clearWay = false;
      }
    }
  } else if (travelAngle > 0) {
    // When we divert to the right side, check if obstacle on left side is past robot.
    for (int i = SECTIONS - 1; i >= (SECTIONS / 3); i--) {
      if (Data_Array[0][i] <= GapDist) {
        clearWay = false;
      }
    }
  } else {
    // TO DO.
  }

  if (clearWay) {
    Serial.println("Step 8: Divert Forward 2 -> Step 9: Divert Rotate 2");
    StepSequence = 9;
    
  }
}

void step9_Divert_Rotate2() {
  //[ROTATE ROBAT BACK ACCORDING TO LASER READINGS]
  if (startTime == -1.0) {
    startTime = millis();
  }
  bool rotateBack = true;
  
  if (travelAngle < 0) {  
  // If we diverted left, rotate to the right until the object is within view again.
  // [ROBOT ROTATE RIGHT]
    for (int i = (SECTIONS / 2) - (SECTIONS / 4); i <= (SECTIONS / 2); i++) {
      if (Data_Array[2][i]  > GapDist) {
        rotateBack = false;
      }
    }
  } else if (travelAngle > 0) {
    // If we diverted right, rotate left until the object is within view again.
    // [ROBOT ROTATE LEFT]
    for (int i = (SECTIONS / 2) + (SECTIONS / 4); i >= (SECTIONS / 2); i--) {
      if (Data_Array[0][i]  > GapDist) {
        rotateBack = false;
      }
    }
  } else {
    // TO DO.
  }

  if (rotateBack) {
    Serial.println("Step 9: Divert Rotate 2 -> Step 10: Divert Forward 3");
    StepSequence = 10;
    
  }
}

void step10_Divert_Forward3() {
  // [ROBOT GOES MOVES FORWARD].
  if (turnTime3 == -1.0) {
    turnTime3 = millis() - startTime;
    totalTurnTime = abs(turnTime1 - turnTime2 - turnTime3);
    turnTime1 = -1.0;
    turnTime2 = -1.0;
    turnTime3 = -1.0;
    startTime = -1.0;
  }
  
  // Check for any obstacles on divert path.
  //step2_ClearCheck("10");
  
  bool clearWay = true;
  if (travelAngle < 0) {
    // When we divert to left side, check if obstacle on right side is past robot.
    for (int i = SECTIONS / 2; i < SECTIONS; i++) {
      if (Data_Array[2][i] <= GapDist) {
        clearWay = false;
      }
    }
  } else if (travelAngle > 0) {
    // When we divert to the right side, check if obstacle on left side is past robot.
    for (int i = SECTIONS / 2; i >= 0; i--) {
      if (Data_Array[0][i] >= GapDist + DistBuffer) {
        clearWay = false;
      }
    }
  } else {
    // TO DO.
  }

  if (clearWay) {
    Serial.println("Step 10: Divert Forward 3 -> Step 11: Return to Path");
    StepSequence = 11;
    
  }
}

void step11_ReturnToPath() {
  float currentTime = 0;
  //[ROTATE ROBAT BACK ACCORDING TO LASER READINGS]
  /*if (startTime == -1.0) {
    startTime = millis();
  }

  currentTime = millis() - startTime;
  
  bool rotateBack = false;*/
 /* 
  if (travelAngle < 0) {  
  // If we diverted left, rotate to the left until right side is clear of the object.
  // [ROBOT ROTATE LEFT]
//    for (int i = SECTIONS / 2; i < SECTIONS; i++) {
//      if (Data_Array[2][i]  <= GapDist) {
//        rotateBack = false;
//      }
//    }
  } else if (travelAngle > 0) {
    // If we diverted right, rotate right until left side is clear.
    // [ROBOT ROTATE RIGHT]
//    for (int i = SECTIONS - 1; i >= (SECTIONS / 2); i--) {
//      if (Data_Array[0][i]  <= GapDist) {
//        rotateBack = false;
//      }
//    }
  } else {
    // TO DO.
  }

  if (currentTime >= totalTurnTime) {
    rotateBack = true;
  }
  
  if (rotateBack) {
    startTime = -1.0;*/
    delay(2000);
    Serial.println("Step 11: Return to Path -> Step 1: Running");
    StepSequence = 1;
    
  //}
  
}
