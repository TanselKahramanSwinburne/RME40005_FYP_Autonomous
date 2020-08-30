// Laser Module Code
// Author:  Ryan Pritchard
//          Tansel A Kahraman
// Last Modified: 30/08/2020
// Description:
/* Reads in values from the YDLIDAR X2 Laser Module
 *  colculates distance and angle values from the messages
 *  collates data into a 360° array.
 *  Divides visibile area into set number of zones and sections to work in.
 *  Allocates angle data (Angle and distance) into the sections to provide a work area for the path planning.
 *  
 */

 
void SetupLaserComms(){
  // Setup motor control on the YLIDAR.
  pinMode(MOTOR, OUTPUT);
  analogWrite(MOTOR, 255);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  //Initialise AngleData array with -1
  for(int f = 0; f<360; f++){
    AngleData[f] = -1;
  }
}

//Sets up zone and section spacings and data array
void SetupDataConfig(){
  Serial.println("VFH Specifications");
  delay(500);
  
  // Printing Zone Specifications
  Serial.print("Zones: " + String(ZONES));

  Zone_Spacing = (END_ANGLE - START_ANGLE) / ZONES;
  Serial.print(" | Zone Spacing: " + String(Zone_Spacing) + " | ");

  for (int i = 0; i < ZONES; i++) {
    zRange1 = String((i * Zone_Spacing) + START_ANGLE);
    zRange2 = String((((i + 1) * Zone_Spacing) - 1) + START_ANGLE);
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
      Data_Array[i][j] = -1.0;
      Serial.print("["+String(Data_Array[i][j]) + "]");
    }
    Serial.println();
  }

  Serial.println("E.g. Data_Array[1][2] - > Zone 1, Section 2 -> (1 * Zone_Spacing) + (2 * Section_Spacing) , ((1 * Zone_Spacing) + (3 * Section_Spacing) - 1), -> " + String((1 * Zone_Spacing) + (2 * Section_Spacing)) + ", " + (((1 * Zone_Spacing) + (3 * Section_Spacing) - 1)));
  
}


void CheckMessage(const byte inByte){
  static int sample_size = 0;
  static byte input_buffer[100];
  static int input_pos = 0;
  static bool newFlag = false;
  
  switch(state){
    case NONE:
      state = FIRST_BYTE;
      break;
      
    case FIRST_BYTE:
      if(inByte == INITIAL_BYTE){
        state = SECOND_BYTE;
      }
      break;
      
    case SECOND_BYTE:
      if(inByte == SECONDARY_BYTE){
        state = TYPE;
      }
      else{
        state = FIRST_BYTE;
      }
      break;
      
    case TYPE:
      if(inByte == RUN_MESSAGE){
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
      state = GET_DATA;
      input_pos = 0;
     }
      break;      
      
   case GET_DATA:
    if(newFlag){
    if(inByte == SECONDARY_BYTE){
      state = TYPE;
      break;
    }
    else{
      newFlag = false;
    }
   }
   if(inByte == INITIAL_BYTE){
    newFlag = true; //flagging the start of a potential new message
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

    //End angle is over the 360° threshold and requires correction to determine correct number of intermediate angles
    if(AngleLSA < AngleFSA){
      AngleLSA += 360.0;
    }
    
    
    float angleDiff = AngleLSA - AngleFSA;
    int ss_1 = samples_sent - 1;
    float frac = angleDiff / float(ss_1);
    float frac_a;
    //Serial.println("FSA: " + String(AngleFSA));
   // Serial.println("frac: " + String(frac));
    
    for(int a = 0; a < samples_sent; a++) {
      //calculate intermediate angle for current sample
      frac_a = frac * a;
      IntAngle[a] = frac_a + AngleFSA;
      
      //Serial.println("Int Angle " + String(a) + ": " + String(IntAngle[a]));
    
      //calculate correction angle for the current sample
      if (Distance[a] == 0) {
        AngCorrectk[a] = 0;
        //Serial.println("Angle " + String(b) + " has distance 0, Correction is not applied");
      } else {
        Numerator = 155.3-Distance[a];
        Denominator = 155.3*Distance[a];
        internalFunc = 21.8*(155.3 - Distance[a])/(155.3*Distance[a]);
        tanFunc = atan(21.8*(155.3 - Distance[a])/(155.3*Distance[a]));
        AngCorrect = 180*tanFunc/PI;//180*atan(21.8*(155.3 - Distance[b])/(155.3*Distance[b]))/PI;
        
       // Serial.println("Num: " + String(Numerator) + " Denom: " + String(155.3*Distance[b]) + " InternalFunc: " + String(internalFunc) + " tanFunc: " + String(tanFunc) + " AngCorrect: " + String(AngCorrect) );
        AngCorrectk[a] = 180*atan(21.8*(155.3 - Distance[a])/(155.3*Distance[a]))/PI; //Answer is in radians so multiply by 180/PI

        
        //Serial.println("Correction " + String(b) + ": " + String(AngCorrectk[b]) + " Distance: " + String(Distance[b]));
      }

      //calucalte total corrected angle and assign distance for current sample
      
      Anglel[a] = IntAngle[a] + AngCorrectk[a];
      if(Anglel[a] >= 360.0){
        Anglel[a] -= 360;
      }
      if(round(Distance[a]) > 0){
        AngleData[round(Anglel[a])] = round(Distance[a]);
      }
      else{ //object is too close to read or cannot be read - error
        AngleData[round(Anglel[a])] = -2;
      }
    }   
}

void ReceivedPacket(byte DataArray[], int sample_size) {  //read the length, starting angle, end angle, check code and then all the data
  String assessment="Bad";   
  int samples_sent;
  long starting_angle_LSB, starting_angle_MSB, end_angle_LSB, end_angle_MSB;
    
  int check_code_LSB, check_code_MSB;  
  int left_column[50], right_column[50];  
  int left_checksum=0;  
  int right_checksum=0;    

  samples_sent = sample_size;
    
  starting_angle_LSB=DataArray[0];  

  starting_angle_MSB=DataArray[1];   

  end_angle_LSB=DataArray[2];   

  end_angle_MSB=DataArray[3];   
  
  check_code_LSB=DataArray[4];  
  
  check_code_MSB=DataArray[5];    
 
  // Perform checksum      
  left_checksum=left_checksum ^ INITIAL_BYTE;  //170
  right_checksum=right_checksum ^ SECONDARY_BYTE; //85  
    
  left_checksum=left_checksum ^ starting_angle_LSB;
  right_checksum=right_checksum ^ starting_angle_MSB;  
  
  for (int j = 0; j < ((samples_sent*2) + 6 - 1); j += 2)  
  {  
    left_checksum=left_checksum ^ DataArray[j + 6];  
    right_checksum=right_checksum ^ DataArray[j+1 + 6];  
  }
    
  left_checksum=left_checksum ^ 0;
  right_checksum=right_checksum ^ samples_sent;   
  
  left_checksum=left_checksum ^ end_angle_LSB;  
  right_checksum=right_checksum ^ end_angle_MSB;  

  //Serial.println("Orig CSM LSB: " + String(check_code_LSB) + " Current CSM LSB: " + String(left_checksum));
  //Serial.println("Orig CSM MSB: " + String(check_code_MSB) + " Current CSM MSB: " + String(right_checksum));
  
  if (left_checksum == check_code_LSB && right_checksum == check_code_MSB) {
    assessment="Good";  
    //Serial.println("Good Checksum");
    ProcessData(DataArray, samples_sent); //message is complete and correct so process the data to find angles and distances
  }

  else {
    //Serial.println(F("Bad Checksum!"));
  } 
}

void UpdateDataArray(){
  int range1, range2;
  uint16_t lowestValue = 65535;
  
  for(int i=0; i < ZONES; i++){
    for(int j=0; j < SECTIONS; j++){
      
      range1 =(i * Zone_Spacing) + (j * Section_Spacing);
      range2 =(i * Zone_Spacing) + ((j + 1) * Section_Spacing);
      
      for(int k = range1; k < range2; k++){
        if (AngleData[k] < lowestValue) {
          //Serial.println("AngleData: " + String(AngleData[k]));
          lowestValue = AngleData[k];
        }
      }
      //Serial.println("Lowest Value: " + String(lowestValue)); 
      Data_Array[i][j] = lowestValue;
      lowestValue = 65535;
      
    }
  }
  
}
