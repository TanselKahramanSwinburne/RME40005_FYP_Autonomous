#include <math.h>

#define BAUD_RATE 115200
#define MOTOR 3
#define SZ_SIZE 13
#define PRINT_TIME 500
#define GREEN_ZONE 3000
#define YELLOW_ZONE 2000
#define RED_ZONE 500
#define RESET_LOOP 5

int incomingByte;   
int resetLoopCounter = 0;

float sections[SZ_SIZE];
float sectionIncrement = 0;
float saveTime = 0;
float printTime;

char zones[SZ_SIZE];

bool isScanning = true;
bool first_byte_received;  
bool second_byte_received;  
bool runPacket = false;
 
  
void setup() {  
  Serial.begin(BAUD_RATE);
    
  first_byte_received=false;  
  second_byte_received=false;

  // Setup motor control on the YLIDAR.
  pinMode(MOTOR, OUTPUT);
  analogWrite(MOTOR, 255);

  // Define how big a section is as per the section size.
  sectionIncrement = (360 - 0) / SZ_SIZE;

  // Initialise the sections array.
  for (int i = 0; i < SZ_SIZE; i++) {
    sections[i] = -1;
  }
}  

// Function for saving data into the relevant section.
void SaveSection(float angle, float distance) {
  // Loop through the sections array.
  for (int i = 0; i < SZ_SIZE; i++) {
    // Check different ranges according to the section increment. 
    if ((angle >= (50 + (i * sectionIncrement))) && (angle < (50 + ((i + 1) * sectionIncrement)))) {
      // Only update the value if the array is in initialise mode (i.e. value 0), or if the current reading is less than what is stored in the array.
      if (((distance > 0) && (distance < sections[i])) || sections[i] < 0) {
        sections[i] = distance;
        //Serial.println("Saving Section[" + String(i) + "]: " + String(distance)); 
      }
    }
  }
}

void SaveZone() {
  for (int i = 0; i < SZ_SIZE; i++) {
    if ((sections[i] > GREEN_ZONE) || ((sections[i] > YELLOW_ZONE) && (sections[i] <= GREEN_ZONE))) {
      zones[i] = 'G'; 
    } else if ((sections[i] > RED_ZONE) && (sections[i] <= YELLOW_ZONE)) {
      zones[i] = 'Y';
    } else if ((sections[i] <= RED_ZONE) && (sections[i] != -1)) {
      zones[i] = 'R';
    } else if (sections[i] == -1) {
      zones[i] = 'x';
    }
  }
}


void CheckMessage() {
  // Poll for our expected 3 bytes that reflect the packet header.  
  if (Serial.available()) {   // If anything comes in Serial (USB),  
    incomingByte=Serial.read();  
    if (!first_byte_received) {  //check if first byte 
      if (incomingByte==170) {
        first_byte_received=true;  //170 DEC = AA Hex
      }
    } else if (!second_byte_received) {  //check if second byte
      if (incomingByte==85) {  //85 Dec = 55 Hex
        second_byte_received=true;  
      } else {  
        first_byte_received=false;  
      }  
    } else { //it's a byte of something, record it, 1 is a start data packet, 0 is a normal packet
      if (incomingByte==0) {  
        first_byte_received=false;  //we're past the first and second byte so reset the checks in case we get an overwritten message or something
        second_byte_received=false;  
        if (runPacket == false) {
          ReceivePacket();  //read the data
        }
      } else {  
        first_byte_received=false;  
        second_byte_received=false;  
      }  
    }  
  }  
}


void ReceivePacket() {  //read the length, starting angle, end angle, check code and then all the data
  String assessment="Bad"; 
 
  float Distance[50];
  float IntAngle[50];
  float AngCorrectk[50]; 
  float Anglel[50];
  
  long samples_sent, starting_angle_LSB, starting_angle_MSB, end_angle_LSB, end_angle_MSB;
    
  int check_code_LSB, check_code_MSB;  
  int left_column[50], right_column[50];  
  int sample_bytes[100];
  int left_checksum=0;  
  int right_checksum=0;    
  
  while (!Serial.available());  //wait for the data to be available in the serail buffer, can also be written While(Serial.available == 0), serial available = number of bytes in the buffer
  samples_sent=Serial.read(); //data is in so pulls out first byte from serial buffer - which is the number of samples in the packet?
  
//  Serial.print("samples Sent: ");
//  Serial.print(samples_sent,HEX);
//  Serial.print(" | ");  
  
  // Expect no more than 40 really, but just in case, just punt if more than 50.  
  if (samples_sent>50) {      
    return;  
  }  
    
  while (!Serial.available());  
  starting_angle_LSB=Serial.read();  
//  Serial.print("Starting Angle LSB: ");
//  Serial.print(starting_angle_LSB,HEX);
//  Serial.print(" | ");
  while (!Serial.available());  
  starting_angle_MSB=Serial.read();  
//  Serial.print("Starting Angle MSB: ");
//  Serial.print(starting_angle_MSB,HEX);
//  Serial.print(" | ");
  while (!Serial.available());  
  end_angle_LSB=Serial.read();  
//  Serial.print("End Angle LSB: ");
//  Serial.print(end_angle_LSB,HEX);
//  Serial.print(" | ");
  while (!Serial.available());  
  end_angle_MSB=Serial.read();  
//  Serial.print("End Angle MSB: ");
//  Serial.print(end_angle_MSB,HEX);
//  Serial.println();
  while (!Serial.available());  
  check_code_LSB=Serial.read(); 
//  Serial.print("Check Code LSB " + String(check_code_LSB));
//  Serial.println(); 
  while (!Serial.available());  
  check_code_MSB=Serial.read();  
//  Serial.print("Check Code MSB " + String(check_code_MSB));
//  Serial.println();
 
  for (int i = 0; i < ((2*samples_sent)); i++) {  //here is the actual data - we then need to perform our calculations to get distance and angle
    while (!Serial.available());    
    sample_bytes[i]=Serial.read(); 
    //Serial.print("Sample " + String(i) + ": ");
    //Serial.println(sample_bytes[i], HEX); //should print out eg sample 1 byte 1: 182
  }  

  // Perform checksum      
  left_checksum=left_checksum ^ 170;  
  right_checksum=right_checksum ^ 85;  
    
  left_checksum=left_checksum ^ starting_angle_LSB;  
  right_checksum=right_checksum ^ starting_angle_MSB;  
  
  for (int j = 0; j < ((samples_sent*2)-1); j += 2)  
  {  
    left_checksum=left_checksum ^ sample_bytes[j];  
    right_checksum=right_checksum ^ sample_bytes[j+1];  
  }  
    
  left_checksum=left_checksum ^ 0;  
  right_checksum=right_checksum ^ samples_sent;   
  
  left_checksum=left_checksum ^ end_angle_LSB;  
  right_checksum=right_checksum ^ end_angle_MSB;  

   
  if (left_checksum==check_code_LSB&&right_checksum==check_code_MSB) {
    assessment="Good";  
  }
  //Serial.println(assessment + ":  Samples Sent:  " + String(samples_sent) + " Checksum: " + String(check_code_LSB) + ", " + String(check_code_MSB) + "  New Checksum: " + String(left_checksum) + ", " + String(right_checksum) ); 
  
  if (assessment == "Good") {  
  // All data is little Endian, so For a sample where the incoming data is S1B1 S1B2 the correct data is then Data1 = S1B2 S1B1 (Achieved by left shifting S1B2 over 8 bits using << 8)
  // (Sample 1 Byte 1)
  // Distance calculation given on the datasheet is then Distance 1 (D1) = Data1/4

    for(int k = 0; k < samples_sent; k++) {
      int msb, lsb;
      lsb = k*2;
      msb = k*2 + 1;
      
      Distance[k] = (sample_bytes[msb] << 8) + sample_bytes[lsb];
    //  Serial.print("Data" + String(i) + ": " + String(Distance[i]));
    //  Serial.println();
      Distance[k] = Distance[k]/4;
    //  Serial.print("Distance: " + String(Distance[i]));
    //  Serial.println();
    }
    
    //anglefsa is the first sample point of data - angle corresponding to that first sample 
    //Angle Formula
    //initial angles
    //AngleFSA = (FSA >> 1)/64
    //AngleLSA = (LSA << 1)/64
  
    float AngleFSA = float((((starting_angle_MSB << 8) + starting_angle_LSB) >> 1)) / 64;
    float AngleLSA = float((((end_angle_MSB << 8) + end_angle_LSB) >> 1)) / 64;
    //long AngleFSA = (((starting_angle_MSB << 8) + starting_angle_LSB) >> 1) / 64;
    //long AngleLSA = (((end_angle_MSB << 8) + end_angle_LSB) >> 1) / 64;
    
//    Serial.print("Starting Angle: ");
//    Serial.print(AngleFSA, DEC);
//    Serial.println();
//    Serial.print("Ending Angle: ");
//    Serial.print(AngleLSA, DEC);
//    Serial.println();
    
    //First level analysis
    //Intermediate Anglei: ((LSA - FSA)/(LSN - 1))*(i - 1) + AngleFSA (where j is the piece of data and LSN is the number of samples in the message.)
    
       
    for(int a = 0; a < samples_sent; a++) {
      float angleDiff = AngleLSA - AngleFSA;
      int ss_1 = samples_sent - 1;
      float frac = angleDiff / float(ss_1);
      float frac_a = frac * a;
      IntAngle[a] = frac_a + AngleFSA;
      //Serial.println("Angle Diff: " + String(angleDiff) + " | Samples Sent - 1: " + String(ss_1) + " | Frac: " + String(frac) + " | j: " + String(a) + " | Frac*j: " + String(frac_a) + " | iAng: " + String(IntAngle[a]));
    }
    
    //Second Level Analysis - take the "first angle, k = 1, to be the initial angle AngleFSA
    //Angle Correction: Anglek = Anglek + AngleCorrectk
    //AngCorrectk = 0 if Distancek is 0, otherwise: arctan(21.8 * (155.3 - Distancek) / (155.3*Distancek))
    
    
  
    
    for (int b = 0; b < samples_sent; b++) {
      if (Distance[b] == 0) {
        AngCorrectk[b] = 0;
  //      Serial.print("AngCorrect " + String(b) + ": " + String(AngCorrectk[b]));
  //      Serial.println();
      } else {
        AngCorrectk[b] = 180*atan(21.8*(155.3 - Distance[b])/(155.3*Distance[b])/PI); //Answer is in radians so multiply by 180/PI
//        Serial.print("AngCorrect " + String(b) + ": " + String(AngCorrectk[b]));
//        Serial.println();
      }
    }
      
    for (int c = 0; c < samples_sent; c++) {
      Anglel[c] = IntAngle[c] + AngCorrectk[c];
      
      if (Anglel[c] < 15) {
        resetLoopCounter++;
        if (resetLoopCounter >= RESET_LOOP) {
          ResetSections();
          resetLoopCounter = 0;
        }
      }
      
      SaveSection(Anglel[c], Distance[c]);
      //Serial.println("Angle " + String(Anglel[c]) + ": " + String(Distance[c]));
    }
    
        
  } else {
    Serial.println("Bad Checksum!");  
  }
   while (Serial.available()){Serial.read();}  
   runPacket = false; 
}  


void ResetSections() {
  for (int i = 0; i < SZ_SIZE; i++) {
    sections[i] = -1;
  } 
}


void PrintZones() {
   // If we haven't started timing, start.
  if (saveTime == 0) {
    saveTime = millis();
  }

  // Calculate how long it has been before we clear the data.
  printTime = millis() - saveTime;

  // If it has been longer than the defined clear time, clear the data.
  if (printTime > PRINT_TIME) {
    for (int i = 0; i < SZ_SIZE; i++) {
      Serial.print("["+String(zones[i])+"]");
    }
    Serial.println(); 
    saveTime = millis();
  }
}
void loop() {  
  CheckMessage();
  SaveZone();
  PrintZones();
}  
  
