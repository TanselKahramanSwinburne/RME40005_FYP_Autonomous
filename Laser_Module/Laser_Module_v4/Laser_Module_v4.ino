#include <math.h>

#define BAUD_RATE 115200
#define MOTOR 3
#define PRINT_TIME 500

int incomingByte;   

float saveTime = 0;
float printTime;

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

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
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
   
  
  // Expect no more than 40 really, but just in case, just punt if more than 50.  
  if (samples_sent>50) {      
    return;  
  }  
    
  while (!Serial.available());  
  starting_angle_LSB=Serial.read();  
  while (!Serial.available());  
  starting_angle_MSB=Serial.read();  
  while (!Serial.available());  
  end_angle_LSB=Serial.read();  
  while (!Serial.available());  
  end_angle_MSB=Serial.read();  
  while (!Serial.available());  
  check_code_LSB=Serial.read(); 
  while (!Serial.available());  
  check_code_MSB=Serial.read();  
 
  for (int i = 0; i < ((2*samples_sent)); i++) {  //here is the actual data - we then need to perform our calculations to get distance and angle
    while (!Serial.available());    
    sample_bytes[i]=Serial.read(); 
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
  
  if (assessment == "Good") {  
  // All data is little Endian, so For a sample where the incoming data is S1B1 S1B2 the correct data is then Data1 = S1B2 S1B1 (Achieved by left shifting S1B2 over 8 bits using << 8)
  // (Sample 1 Byte 1)
  // Distance calculation given on the datasheet is then Distance 1 (D1) = Data1/4

    for(int k = 0; k < samples_sent; k++) {
      int msb, lsb;
      lsb = k*2;
      msb = k*2 + 1;
      
      Distance[k] = (sample_bytes[msb] << 8) + sample_bytes[lsb];
      Distance[k] = Distance[k]/4;
    }
  
    float AngleFSA = float((((starting_angle_MSB << 8) + starting_angle_LSB) >> 1)) / 64;
    float AngleLSA = float((((end_angle_MSB << 8) + end_angle_LSB) >> 1)) / 64;
       
    for(int a = 0; a < samples_sent; a++) {
      float angleDiff = AngleLSA - AngleFSA;
      int ss_1 = samples_sent - 1;
      float frac = angleDiff / float(ss_1);
      float frac_a = frac * a;
      IntAngle[a] = frac_a + AngleFSA;
    }
    
    for (int b = 0; b < samples_sent; b++) {
      if (Distance[b] == 0) {
        AngCorrectk[b] = 0;
      } else {
        AngCorrectk[b] = 180*atan(21.8*(155.3 - Distance[b])/(155.3*Distance[b])/PI); //Answer is in radians so multiply by 180/PI
      }
    }
      
    for (int c = 0; c < samples_sent; c++) {
      Anglel[c] = IntAngle[c] + AngCorrectk[c];
    
      Serial.println("Angle " + String(Anglel[c]) + ": " + String(Distance[c]));
    }

    
  } else {
    Serial.println(F("Bad Checksum!"));  
  }
   while (Serial.available()){Serial.read();}  
   runPacket = false; 
}  



void loop() {  
  CheckMessage();

}  
  
