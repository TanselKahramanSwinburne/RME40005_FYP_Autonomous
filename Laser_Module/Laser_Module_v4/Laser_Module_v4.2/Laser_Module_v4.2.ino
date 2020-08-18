#include <math.h>

#define BAUD_RATE 115200
#define MOTOR 3
#define PRINT_TIME 500

int incomingByte; 

int DataArray[360];
int IncomingMessage[100];

int MsgLen = 0;
int MsgLoc = 0;
bool MsgReading = 0;
bool MsgWait = 0;
bool MsgDone = 0;
bool DataCalced = 1;
int DataSamples = 0;

long samples_sent, starting_angle_LSB, starting_angle_MSB, end_angle_LSB, end_angle_MSB;
long Data_starting_angle_LSB, Data_starting_angle_MSB, Data_end_angle_LSB, Data_end_angle_MSB;
int check_code_LSB, check_code_MSB;  

int sample_bytes[100];
int sampled_bytes[100];

bool first_byte_received;  
bool second_byte_received; 






void setup() {
  Serial.begin(BAUD_RATE);
  Serial1.begin(BAUD_RATE);
    
  first_byte_received=false;  
  second_byte_received=false;

  // Setup motor control on the YLIDAR.
  pinMode(MOTOR, OUTPUT);
  analogWrite(MOTOR, 255);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  for(int e = 0; e<360; e++){
    DataArray[e] = -1;
  }
}

void CheckMessage() {
  Serial.println("Check Msg Start");
  // Poll for our expected 3 bytes that reflect the packet header.  
  if (Serial1.available()) {   // If anything comes in Serial (USB),  
    incomingByte = Serial1.read();  
    
    if (!first_byte_received) {  //check if first byte 
      if (incomingByte==170) {
        first_byte_received=true;  //170 DEC = AA Hex
        Serial.println("Check Msg FirstByte Good");
      }
      
    } else if (!second_byte_received) {  //check if second byte
      if (incomingByte==85) {  //85 Dec = 55 Hex
        second_byte_received=true;  
       Serial.println("Check Msg SecondByte Good");
      } else {  
        first_byte_received=false; 
        Serial.println("Check Msg 1"); 
      }  
      
    } else { //it's a byte of something, record it, 1 is a start data packet, 0 is a normal packet
      if ((incomingByte==0) && first_byte_received && second_byte_received) {  
        first_byte_received=false;  //we're past the first and second byte so reset the checks in case we get an overwritten message or something
        second_byte_received=false;  
       Serial.println("Check Msg ReceivePacket");
        ReceivePacketStart();  //read the header data and any available sample data
        
      } else {  //cancel operation
        first_byte_received=false;  
        second_byte_received=false;  
        Serial.println("Check Msg Cancel");
      }  
    }  
  }
  Serial.println("Check Msg End");  
}

void ReceivePacketStart(){//read the length, starting angle, end angle, check sum and all available data at this stage
  int incomingData = 0;

  Serial.println("Received");

  while (!Serial1.available()); //wait for the data to be available in the serail buffer, can also be written While(Serial.available == 0), serial available = number of bytes in the buffer
  samples_sent=Serial1.read(); //data is in so pulls out first byte from serial buffer - which is the number of samples in the packet
  Serial.println(String(samples_sent));
  
  if (samples_sent > 40) { //more data than expected, boot the message and wait for the next one
    return;  
  }  
  else{
    MsgLen = 2 * samples_sent;
    MsgLoc = 0;
  }
  
  
  while (!Serial1.available());  
  starting_angle_LSB=Serial1.read(); 
  IncomingMessage[0] = starting_angle_LSB;
  while (!Serial1.available());  
  starting_angle_MSB=Serial1.read();
  IncomingMessage[1] = starting_angle_MSB;
  while (!Serial1.available());  
  end_angle_LSB=Serial1.read();
  IncomingMessage[2] = end_angle_LSB;
  while (!Serial1.available());  
  end_angle_MSB=Serial1.read();
  IncomingMessage[3] = end_angle_MSB;
  while (!Serial1.available());  
  check_code_LSB=Serial1.read(); 
  IncomingMessage[4] = check_code_LSB;
  while (!Serial1.available());  
  check_code_MSB=Serial1.read();   
  IncomingMessage[5] = check_code_MSB;

  MsgLoc = 6;
  MsgLen += 6;
  
  //while(!Serial1.available());
  if((Serial1.available() >0) && (MsgLoc < MsgLen)){
    MsgWait = 0;
    incomingData = Serial1.read();
    Serial.println("four.five");
    if(incomingData == -1){//trying to compensate if we get on overwritten message, look for AA55 to restart the sequence.

    }
    else{
      Serial.println("fifth: MsgLoc: " + String(MsgLoc));
      IncomingMessage[MsgLoc] = incomingData;
      MsgReading = 1;
      MsgWait = 1;
      MsgLoc++;  
    }
  }  
}

void CheckData(){ //reads in new data into the partially filled sample_bytes array, at the next location (MsgLoc)
  int inboundData = 0;
  Serial.println("fifthpointfive");
  while((Serial1.available() > 0) && (MsgLoc < MsgLen)){
    MsgWait = 0;
    inboundData = Serial1.read();
    
    if(inboundData == -1){//trying to compensate if we get on overwritten message, look for AA55 to restart the sequence.

    }
    else{
      Serial.println("Sixth: " + inboundData);
      IncomingMessage[MsgLoc] = inboundData;
      MsgReading = 1;
      MsgWait = 1;
      MsgLoc++;  
    }
  }
  Serial.println("MsgLoc: " + String(MsgLoc) + " | MsgLen: " + String(MsgLen));
  if(MsgLoc == MsgLen){ //reached the end of the message, msg is done and not waiting for any more
    Serial.println("Message Done");
    MsgDone = 1;
    MsgWait = 0; 
  }
}

void CalcData(){ //calc angles and distances
  int Distance[50];
  float IntAngle[50];
  float AngCorrect[50];
  float Angle[50];

  Serial.println("calc begins");
  
  for(int k = 0; k < DataSamples; k++) { //calculate distances
      int msb, lsb;
      lsb = k*2 + 6; //add 6 for the message header
      msb = k*2 + 1 + 6;
     
      Distance[k] = (sampled_bytes[msb] << 8) + sampled_bytes[lsb];
      Distance[k] = Distance[k]/4;
      Serial.println("Distance : " + String(Distance[k]));
    }

    //anglefsa is the first sample point of data - angle corresponding to that first sample 
    //Angle Formula
    float AngleFSA = float((((Data_starting_angle_MSB << 8) + Data_starting_angle_LSB) >> 1)) / 64;
    float AngleLSA = float((((Data_end_angle_MSB << 8) + Data_end_angle_LSB) >> 1)) / 64;

    //First level analysis
    //Intermediate Anglei: ((LSA - FSA)/(LSN - 1))*(i - 1) + AngleFSA (where j is the piece of data and LSN is the number of samples in the message.)

    for(int a = 0; a < DataSamples; a++) {
      float angleDiff = AngleLSA - AngleFSA;
      int ss_1 = DataSamples - 1;
      float frac = angleDiff / float(ss_1);
      float frac_a = frac * a;
      IntAngle[a] = frac_a + AngleFSA;
    }

    //Second Level Analysis - take the "first angle, k = 0, to be the initial angle AngleFSA
    //Angle Correction: Anglek = Anglek + AngleCorrectk
    //AngCorrectk = 0 if Distancek is 0, otherwise: arctan(21.8 * (155.3 - Distancek) / (155.3*Distancek))
    
    for (int b = 0; b < DataSamples; b++) {
      if (Distance[b] == 0) {
        AngCorrect[b] = 0;
      } else {
        AngCorrect[b] = 180*atan(21.8*(155.3 - Distance[b])/(155.3*Distance[b])/PI); //Answer is in radians so multiply by 180/PI
      }
    }

    for (int c = 0; c < DataSamples; c++) {
      
      Angle[c] = IntAngle[c] + AngCorrect[c];

      if((int(Distance[c]) != 0)){
        
        DataArray[int(Angle[c])] = Distance[c];
      }
      else{
        DataArray[int(Angle[c])] = -2;
      }      
    }
  
  DataCalced = 1;
}

void loop() {
  Serial.println("Msgwait: " + String(MsgWait) + " | Reading: " + String(MsgReading));
  if(Serial1.available() > 0){
    if(MsgWait && MsgReading){
      Serial.println("CheckData");
      CheckData();
    }
    else{
      Serial.println("CheckMsg");
      CheckMessage();
    }
  }
  if(MsgDone && MsgReading && !MsgWait){
    Serial.println("ToCalculate");
    
    if(DataCalced){
      DataCalced = 0;
      
      DataSamples = (MsgLen - 6) / 2;
      
      Data_starting_angle_LSB = IncomingMessage[0];
      Data_starting_angle_MSB = IncomingMessage[1];
      Data_end_angle_LSB = IncomingMessage[2]; 
      Data_end_angle_MSB = IncomingMessage[3];      

      memcpy(sampled_bytes, IncomingMessage,1600);  

      CalcData();
    }
    
    MsgDone = 0;
    MsgReading = 0;
  }

  //for(int f = 0; f<360; f++){
    Serial.println("Angle:  | Distance: " + String(DataArray[50]));
  
}
