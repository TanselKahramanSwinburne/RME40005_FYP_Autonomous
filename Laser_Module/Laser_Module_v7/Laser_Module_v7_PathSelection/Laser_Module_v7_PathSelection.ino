#include <HardwareSerial.h>

//Laser Comms Defines
#define BAUD_RATE 115200


//Data Configuration Defines
#define ZONES 3
#define SECTIONS 10
#define START_ANGLE 50
#define END_ANGLE 250


//Data Configuration Variables
int Data_Array[ZONES][SECTIONS];
int Zone_Spacing;
int Section_Spacing;
String zRange1, zRange2, sRange1, sRange2;

//Sets up zone and section spacings and data array
void SetupDataConfig(){
  //Serial.println("VFH Specifications");
  delay(500);
  
  // Printing Zone Specifications
  //Serial.print("Zones: " + String(ZONES));

  Zone_Spacing = (END_ANGLE - START_ANGLE) / ZONES;
  //Serial.print(" | Zone Spacing: " + String(Zone_Spacing) + " | ");

  for (int i = 0; i < ZONES; i++) {
    zRange1 = String((i * Zone_Spacing) + START_ANGLE);
    zRange2 = String((((i + 1) * Zone_Spacing) - 1) + START_ANGLE);
    if (i < (ZONES - 1)) {
      zRange2 += ", ";
    }
    //Serial.print("Zone " + String(i) + ": " + zRange1 + " - " + zRange2);

  }
  //Serial.println();

  // Printing Section Specifications
  //Serial.print("Sections: " + String(SECTIONS));
  
  Section_Spacing = Zone_Spacing / SECTIONS;
  //Serial.println(" | Section Spacing: " + String(Section_Spacing));

  for (int i = 0; i < ZONES; i++) {
    Serial.print("Zone " + String(i) + " Sections: ");
    for (int j = 0; j < SECTIONS; j++) {
      sRange1 = String(((j * Section_Spacing)+ (i * Zone_Spacing)) + START_ANGLE);
      sRange2 = String(((((j + 1) * Section_Spacing) - 1) + (i * Zone_Spacing)) + START_ANGLE);
      if (i < (SECTIONS - 1)) {
      sRange2 += ", ";
      }
      Serial.print("Sec. " + String(j) + ": " + sRange1 + " - " + sRange2);
    }
    Serial.println();
  }

  for (int i = 0; i < ZONES; i++) {
    for (int j = 0; j < SECTIONS; j++) {
      Data_Array[i][j] = 5000.0;
      Serial.print("["+String(Data_Array[i][j]) + "]");
    }
    Serial.println();
  }
  //Serial.println("E.g. Data_Array[1][2] - > Zone 1, Section 2 -> (1 * Zone_Spacing) + (2 * Section_Spacing) , ((1 * Zone_Spacing) + (3 * Section_Spacing) - 1), -> " + String((1 * Zone_Spacing) + (2 * Section_Spacing)) + ", " + (((1 * Zone_Spacing) + (3 * Section_Spacing) - 1)));
}


int StepSequence;
int leftGapIndex = -1;
int rightGapIndex = -1;
int travelAngle;

float t1, t2; // Simulation timers.

void setup() {  
  Serial.begin(BAUD_RATE);
  SetupDataConfig();
  StepSequence = 0;
  t1 = millis();
}  

void loop() {
  simulation();
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

void simulation() {
  t2 = millis() - t1;
  if ((t2 > 3000) && (t2 < 3100)) {
    Data_Array[1][0] = 5000;
    Data_Array[1][1] = 4500;
    Data_Array[1][2] = 3000;
    Data_Array[1][3] = 2000;
    Data_Array[1][4] = 1000;
    Data_Array[1][5] = 500;
    Data_Array[1][6] = 350;
    Data_Array[1][7] = 200;
    Data_Array[1][8] = 100;
    Data_Array[1][9] = 1000;

    Data_Array[0][0] = 800;
    Data_Array[0][1] = 800;
    Data_Array[0][2] = 800;
    Data_Array[0][3] = 200;
    Data_Array[0][4] = 200;
    Data_Array[0][5] = 200;
    Data_Array[0][6] = 200;
    Data_Array[0][7] = 200;
    Data_Array[0][8] = 200;
    Data_Array[0][9] = 200;

    Data_Array[2][0] = 200;
    Data_Array[2][1] = 200;
    Data_Array[2][2] = 200;
    Data_Array[2][3] = 200;
    Data_Array[2][4] = 200;
    Data_Array[2][5] = 700;
    Data_Array[2][6] = 700;
    Data_Array[2][7] = 800;
    Data_Array[2][8] = 200;
    Data_Array[2][9] = 200;

  }
}

void step0_Reset() {
  // [ROBOT STOP MOVING]
  
  StepSequence = 1;
  Serial.println("Step 0: Reset -> Step 1: Running");
}

void step1_Running() {
  // [ROBOT MOVING FORWARD]
  
  // Look for WORST CASE scenario in front zone.
  for (int i = 0; i < SECTIONS; i++) {
    if (Data_Array[1][i] <= 300) {
      StepSequence = 2;
      Serial.println("Step 1: Running -> Step 2: Find Gap");
      break;
    }
  }
}

void step2_FindGap() {
  leftGapIndex = -1;
  rightGapIndex = -1;
  
  // [ROBOT STOP]

  // Look for a three section 500mm gap in the left and right zones.
  for (int i = 0; i < (SECTIONS - 2); i++) {
    if (((Data_Array[0][i] >= 500) &&  (Data_Array[0][i + 1] >= 500) && (Data_Array[0][i + 2] >= 500)) && (leftGapIndex == -1)) {
      leftGapIndex = i + 1;
    }

    if (((Data_Array[2][i] >= 500) &&  (Data_Array[2][i + 1] >= 500) && (Data_Array[2][i + 2] >= 500)) && (rightGapIndex == -1)) {
      rightGapIndex = i + 1;
    }
  }

  // PRINTING DATA ARRAY
  for (int i = 0; i < ZONES; i++) {
    for (int j = 0; j < SECTIONS; j++) {
      Serial.print("["+String(Data_Array[i][j]) + "]");
    }
    Serial.println();
  }
  //-----------------------

  
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

  if ((t2 >= 6000) && (t2 <= 6005)) {
    Serial.println("frontAngle: " + String(frontAngle));
    
    Serial.print("leftTargetAngleLocal: " + String(leftTargetAngleLocal));
    Serial.println(" | leftTravelAngle: " + String(leftTravelAngle));
    
    Serial.print("rightTargetAngleLocal: " + String(rightTargetAngleLocal));
    Serial.println(" | rightTravelAngle: " + String(rightTravelAngle));

    Serial.println("travelAngle: " + String(travelAngle));

    // [ROBOT ROTATE UNTIL COMPASS GOES TO THIS BEARING]
    StepSequence = 4;
    Serial.println("Step 3: Calculate to Gap -> Step 4: Rotate to Gap");
  }
  
  
}

void step4_RotateToGap() {
  // [ROBOT ROTATE UNTIL COMPASS BEARING CHANGES EQUIVALENT TO travelAngle]
  if ((t2 >= 7000)) {
    StepSequence = 1;
    Serial.println("Step 4: Rotate to Gap -> Step 1: Running");

    // Reset data array sim.
    for (int i = 0; i < ZONES; i++) {
      for (int j = 0; j < SECTIONS; j++) {
        Data_Array[i][j] = 5000.0;
        Serial.print("["+String(Data_Array[i][j]) + "]");
      }
      Serial.println();
    }
    //t1 = millis();
  }
}

void step5_RotateToFindGap() {
  // [ROBOT ROTATE UNTIL GAP FOUND]
}
  
