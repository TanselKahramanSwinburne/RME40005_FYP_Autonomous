//#include <math.h>
#include <HardwareSerial.h>
/*
#define BAUD_RATE 115200
#define MOTOR 3
#define PRINT_TIME 500
#define INITIAL_BYTE 170
#define SECONDARY_BYTE 85
#define START_MESSAGE 0
#define ARRAY_SIZE 100 //10 bytes of header + 80 bytes of data (2 bytes per sample) + 10 spare bytes to avoid errors*/
/*
int incomingByte;   




float saveTime = 0;
float printTime;

bool isScanning = true;
bool first_byte_received;  
bool second_byte_received;  
*/
float AngleData[360];
typedef enum { FIRST_BYTE, SECOND_BYTE, TYPE, SAMPLE_NUM, GET_DATA, NONE } states;

states state = NONE;
  
void setup() {  
  Serial.begin(115200);
  //Serial.begin(BAUD_RATE);
  Serial1.begin(115200);  
 // first_byte_received=false;  
 // second_byte_received=false;

  // Setup motor control on the YLIDAR.
  pinMode(3, OUTPUT);
  analogWrite(3, 255);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  for(int f = 0; f<360; f++){
    AngleData[f] = -1;
  }
  
}  

void CheckMessage(const byte inByte){
  static int sample_size = 0;
  static byte input_buffer[100];
  static int input_pos = 0;
  static bool first_byte_received, second_byte_received;
  static bool newFlag = false;
  
  switch(state){
    case NONE:
      state = FIRST_BYTE;
      break;
      
    case FIRST_BYTE:
      if(inByte == 170){
        state = SECOND_BYTE;
      }
      break;
      
    case SECOND_BYTE:
      if(inByte == 85){
        state = TYPE;
      }
      else{
        state = FIRST_BYTE;
      }
      break;
      
    case TYPE:
      if(inByte == 0){
        state = SAMPLE_NUM;     
      }
      else{
        state = FIRST_BYTE;
      }
      break;
      
    case SAMPLE_NUM:
     sample_size = inByte;
     if(sample_size > 40 || sample_size < 1){
      state = FIRST_BYTE;
     }
     else{
      //Serial.println("SampleSize: " + String(sample_size));
      state = GET_DATA;
      input_pos = 0;
     }
      break;      
      
   case GET_DATA:
    if(newFlag){
    if(inByte == 85){
      //Serial.println("issue2");
      state = TYPE;
      break;
    }
    else{
      newFlag = false;
    }
   }
   if(inByte == 170){
    newFlag = true;
   // Serial.println("issue1");
   }
  
   
    if(input_pos < ((2*sample_size) + 6 - 1)){ //data plus starting angle, end angle and checksum, 2 bytes for each
        input_buffer[input_pos] = inByte;
        
        //Serial.println("Loc: " + String(input_pos) + " : Byte: " + String(inByte));
        input_pos++;
      }
    else if(input_pos == ((2*sample_size) + 6 - 1)){//final character of array, process data
        
        input_buffer[input_pos] = inByte;
        //Serial.println("end Loc: " + String(input_pos) + " : Byte: " + String(inByte));
        //PrintData(input_buffer, sample_size);
        //process data
        ReceivedPacket(input_buffer, sample_size);
        
        //reset buffer for next time
        input_pos = 0;
        state = FIRST_BYTE;
      }
    break;
   
  }
}

/*void PrintData(byte DataArray[], int sample_size){
  Serial.print("Size: " + String(sample_size) + " | ");
  for(int i=0; i<(2*sample_size + 6); i++){
    Serial.print(String(DataArray[i]) + " ");
  }
  Serial.println();
}*/
/*
void CheckMessage(const byte inByte){ //make inByte read only
  int sample_size = 0;
  static byte input_buffer[ARRAY_SIZE];
  static int input_pos = 0; //remember the location of the data
  static bool first_byte_received, second_byte_received;
  
  switch (inByte){
    case FIRST_BYTE: //check if first byte
      if(!first_byte_received){
        input_pos = 0;
        input_buffer[input_pos] = inByte;
        first_byte_received = true;
        input_pos++;
      }
      else{//add to array
        if(input_pos < (ARRAY_SIZE - 1){
          input_buffer[input_pos] = inByte;
          input_pos++;
        }
      }
        break;

    case SECOND_BYTE: //check if second byte
      if(!second_byte_received && first_byte_received){
        input_buffer[input_pos] = inByte;
        second_byte_received = true;
        input_pos++;
      }
      else{//add to array
        first_byte_received = false;
        if(input_pos < (ARRAY_SIZE - 1){
          input_buffer[input_pos] = inByte;
          input_pos++;
        }
      }
      break;


    default: //keep adding to array until full
      if(input_pos < (ARRAY_SIZE - 1){
        input_buffer[input_pos] = inByte;
        input_pos++;
        first_byte_received = false;
        second_byte_received = false;
      }
      else if(input_pos == (ARRAY_SIZE - 1){//final character of array, process data
        
        input_buffer[input_pos] = inByte;
        //process data

        //reset buffer for next time
        input_pos = 0;
      }
      else{
        //error
      }
      break;
  }//end of switch
  
}//end of check message
*/

void ProcessData(byte sample_bytes[], int samples_sent){
  float Anglel[50];
  float Distance[50];
  float IntAngle[50];
  float AngCorrectk[50]; 
//Testing variables
  float Numerator;
  float Denominator;
  float internalFunc;
  float tanFunc;
  float AngCorrect;
  
  long starting_angle_LSB, starting_angle_MSB, end_angle_LSB, end_angle_MSB;
 
  starting_angle_LSB=sample_bytes[0];  

  starting_angle_MSB=sample_bytes[1];   

  end_angle_LSB=sample_bytes[2];   

  end_angle_MSB=sample_bytes[3];   
     
  // All data is little Endian, so For a sample where the incoming data is S1B1 S1B2 the correct data is then Data1 = S1B2 S1B1 (Achieved by left shifting S1B2 over 8 bits using << 8)
  // (Sample 1 Byte 1)
  // Distance calculation given on the datasheet is then Distance 1 (D1) = Data1/4

    for(int k = 0; k < ((samples_sent)); k++) {
      int msb, lsb;
      lsb = k*2 + 6;
      msb = k*2 + 1 + 6;
      
      Distance[k] = (sample_bytes[msb] << 8) + sample_bytes[lsb];
     // Serial.println("Distance " + String(k) + ": " + String(Distance[k]) + " MSB " + sample_bytes[msb] + " LSB " + sample_bytes[lsb]);
      Distance[k] = Distance[k]/4;
      //Serial.println("Distance: " + String(Distance[k]));
    }
  
    float AngleFSA = float((((starting_angle_MSB << 8) + starting_angle_LSB) >> 1)) / 64;
    float AngleLSA = float((((end_angle_MSB << 8) + end_angle_LSB) >> 1)) / 64;
    float angleDiff = AngleLSA - AngleFSA;
    int ss_1 = samples_sent - 1;
    float frac = angleDiff / float(ss_1);
   // Serial.println("FSA: " + String(AngleFSA));
   // Serial.println("frac: " + String(frac));
    
    for(int a = 0; a < samples_sent; a++) {
      float frac_a = frac * a;
      IntAngle[a] = frac_a + AngleFSA;
      //Serial.println("Int Angle " + String(a) + ": " + String(IntAngle[a]));
    }
    
    for (int b = 0; b < samples_sent; b++) {
      if (Distance[b] == 0) {
        AngCorrectk[b] = 0;
        //Serial.println("Angle " + String(b) + " has distance 0, Correction is not applied");
      } else {
        Numerator = 155.3-Distance[b];
        Denominator = 155.3*Distance[b];
        internalFunc = 21.8*(155.3 - Distance[b])/(155.3*Distance[b]);
        tanFunc = atan(21.8*(155.3 - Distance[b])/(155.3*Distance[b]));
        AngCorrect = 180*tanFunc/PI;//180*atan(21.8*(155.3 - Distance[b])/(155.3*Distance[b]))/PI;
        
       // Serial.println("Num: " + String(Numerator) + " Denom: " + String(155.3*Distance[b]) + " InternalFunc: " + String(internalFunc) + " tanFunc: " + String(tanFunc) + " AngCorrect: " + String(AngCorrect) );
        AngCorrectk[b] = 180*atan(21.8*(155.3 - Distance[b])/(155.3*Distance[b]))/PI; //Answer is in radians so multiply by 180/PI
        if(AngCorrectk[b] == 0.00 && AngCorrect != 0.00){
         // delay(10000);
        }
        //Serial.println("Correction " + String(b) + ": " + String(AngCorrectk[b]) + " Distance: " + String(Distance[b]));
      }
    }
      
    for (int c = 0; c < samples_sent; c++) {
      Anglel[c] = IntAngle[c] + AngCorrectk[c];
    
      //Serial.println("Angle " + String(c) + ": " + String(Anglel[c]) + ": " + String(Distance[c]));
      //Serial.println("Rounded: " + String(round(Anglel[c])));
      if(Distance[c] > 0.0){
        AngleData[round(Anglel[c])] = Distance[c];
      }
      
      //Serial.println("Saved: Angle " + String(round(Anglel[c])) + ": " + String(AngleData[round(Anglel[c])]));
    }

  //Serial.println("End Message");
    
  //end of calculations if statement */
  
   //while (Serial1.available()){Serial1.read();}  
  
   
}

void ReceivedPacket(byte DataArray[], int sample_size) {  //read the length, starting angle, end angle, check code and then all the data
  String assessment="Bad"; 
 
 /* float Anglel[50];
  float Distance[50];
  float IntAngle[50];
  float AngCorrectk[50]; */
//Testing variables
  /*float Numerator;
  float Denominator;
  float internalFunc;
  float tanFunc;
  float AngCorrect;*/
  
  int samples_sent;
  long starting_angle_LSB, starting_angle_MSB, end_angle_LSB, end_angle_MSB;
    
  int check_code_LSB, check_code_MSB;  
  int left_column[50], right_column[50];  
  int sample_bytes[100];
  int left_checksum=0;  
  int right_checksum=0;    

  samples_sent = sample_size;
  //Serial.println("Start Message");
  
  //while (!Serial1.available());  //wait for the data to be available in the serail buffer, can also be written While(Serial.available == 0), serial available = number of bytes in the buffer
  //samples_sent=Serial1.read(); //data is in so pulls out first byte from serial buffer - which is the number of samples in the packet?
   
  
  // Expect no more than 40 really, but just in case, just punt if more than 50.  
  //if (samples_sent>50) {      
    //return;  
  //}  
    
  starting_angle_LSB=DataArray[0];  

  starting_angle_MSB=DataArray[1];   

  end_angle_LSB=DataArray[2];   

  end_angle_MSB=DataArray[3];   
  
  check_code_LSB=DataArray[4];  
  
  check_code_MSB=DataArray[5];    
 
  /*for (int i = 0; i < ((2*samples_sent)); i++) {  //here is the actual data - we then need to perform our calculations to get distance and angle
  
    sample_bytes[i]=DataArray[i+6]; 
    //Serial.print(String(DataArray[i+6]) + " ");
  } */ 
  /*Serial.print("Data Array:   ");

  for (int i = 0; i < ((2*sample_size)); i++) {  //here is the actual data - we then need to perform our calculations to get distance and angle
  
    Serial.print(String(DataArray[i+6]) + " ");
  }  
  Serial.println();

  Serial.print("Samples Sent: ");
  for (int i = 0; i < ((2*samples_sent)); i++) {  //here is the actual data - we then need to perform our calculations to get distance and angle
  
    Serial.print(String(sample_bytes[i]) + " ");
  }  
  Serial.println();
  */
  // Perform checksum      
  left_checksum=left_checksum ^ 170;  //170
  right_checksum=right_checksum ^ 85;  
    
  left_checksum=left_checksum ^ starting_angle_LSB;  //170
  right_checksum=right_checksum ^ starting_angle_MSB;  
  
  for (int j = 0; j < ((samples_sent*2) + 6 - 1); j += 2)  
  {  
    left_checksum=left_checksum ^ DataArray[j + 6];  
    right_checksum=right_checksum ^ DataArray[j+1 + 6];  
  }  //109
    
  left_checksum=left_checksum ^ 0;  //109
  right_checksum=right_checksum ^ samples_sent;   
  
  left_checksum=left_checksum ^ end_angle_LSB;  
  right_checksum=right_checksum ^ end_angle_MSB;  

  //Serial.println("Orig CSM LSB: " + String(check_code_LSB) + " Current CSM LSB: " + String(left_checksum));
  //Serial.println("Orig CSM MSB: " + String(check_code_MSB) + " Current CSM MSB: " + String(right_checksum));
  
  if (left_checksum==check_code_LSB&&right_checksum==check_code_MSB) {
    assessment="Good";  
    //Serial.println("Good Checksum");
    ProcessData(DataArray, samples_sent);
  }

  else {
    //Serial.println(F("Bad Checksum!"));
  } 
}




void loop() {  

  static byte data[100];
  static int i = 0;
  byte incomingByte;
  static int x = 0;
while(Serial1.available() > 0){
    CheckMessage(Serial1.read());
//  incomingByte = Serial.read();
//  if(incomingByte == 170
//  data[i] = incomingByte;
} //else{
  Serial.println("Angle " + String(x) + ": " + String(AngleData[x]));
  x++;
  if(x>= 360){
    x = 0;
  }

//Serial.println("Looping");
//delay(200);

//}
//Serial.println("End");


  
  // put your main code here, to run repeatedly:
  //Serial.println("Helloworld");
  /*if(Serial.available() > 0){
    //Serial.println("Data");
    //Data[i]= Serial.read();
    byte data = Serial.read();
    Serial.println(String(data));
    //Serial.println();
    i++;
  }
  Serial.println("Count: " + String(i));
  delay(100);*/
  
 
  

}  
  
