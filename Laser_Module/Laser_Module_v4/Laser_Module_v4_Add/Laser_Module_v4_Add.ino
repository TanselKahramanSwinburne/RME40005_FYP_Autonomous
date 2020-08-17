#include <math.h>

#define BAUD_RATE 115200
#define ZONES 3
#define SECTIONS 10
#define START_ANGLE 50
#define END_ANGLE 250

float Data_Array[ZONES][SECTIONS];
float Zone_Spacing;
float Section_Spacing;
String zRange1, zRange2, sRange1, sRange2;

void setup() {
  
  
  Serial.begin(BAUD_RATE);
  Serial.println("VFH Specifications");
  delay(500);
  
  // Printing Zone Specifications
  Serial.print("Zones: " + String(ZONES));

  Zone_Spacing = (START_ANGLE + END_ANGLE) / ZONES;
  Serial.print(" | Zone Spacing: " + String(Zone_Spacing) + " | ");

  for (int i = 0; i < ZONES; i++) {
    zRange1 = String(i * Zone_Spacing);
    zRange2 = String(((i + 1) * Zone_Spacing) - 1);
    if (i < (ZONES - 1)) {
      zRange2 += ", ";
    }
    Serial.print("Zone " + String(i) + ": " + zRange1 + " - " + zRange2);

  }
  Serial.println();

  // Printing Section Specifications
  Serial.print("Sections: " + String(SECTIONS));
  
  Section_Spacing = Zone_Spacing / SECTIONS;
  Serial.println(" | Section Spacing: " + String(Section_Spacing));

  for (int i = 0; i < ZONES; i++) {
    Serial.print("Zone " + String(i) + " Sections: ");
    for (int j = 0; j < SECTIONS; j++) {
      sRange1 = String((j * Section_Spacing)+ (i * Zone_Spacing));
      sRange2 = String((((j + 1) * Section_Spacing) - 1) + (i * Zone_Spacing));
      if (i < (SECTIONS - 1)) {
      sRange2 += ", ";
      }
      Serial.print("Sec. " + String(j) + ": " + sRange1 + " - " + sRange2);
    }
    Serial.println();
  }

  for (int i = 0; i < ZONES; i++) {
    for (int j = 0; j < SECTIONS; j++) {
      Data_Array[i][j] = -1.0;
      Serial.print("["+String(Data_Array[i][j]) + "]");
    }
    Serial.println();
  }

  Serial.println("E.g. Data_Array[1][2] - > Zone 1, Section 2 -> (1 * Zone_Spacing) + (2 * Section_Spacing) , ((1 * Zone_Spacing) + (3 * Section_Spacing) - 1), -> " + String((1 * Zone_Spacing) + (2 * Section_Spacing)) + ", " + (((1 * Zone_Spacing) + (3 * Section_Spacing) - 1)));
  
}



void loop() {

  
}
