
#include "QMC5883LCompass.h"

QMC5883LCompass compass;
const int ArraySize = 25;
int AveragingXArray[ArraySize];
int AveragingYArray[ArraySize];
int AveragingLoop = 0;

void setup() {
  Serial.begin(9600);
  compass.init();

  /*
   *   call setSmoothing(STEPS, ADVANCED);
   *   
   *   STEPS     = int   The number of steps to smooth the results by. Valid 1 to 10.
   *                     Higher steps equals more smoothing but longer process time.
   *                     
   *   ADVANCED  = bool  Turn advanced smmothing on or off. True will remove the max and min values from each step and then process as normal.
   *                     Turning this feature on will results in even more smoothing but will take longer to process.
   *                     
   */
  compass.setSmoothing(10,true);  
}

void loop() {
  int x, xAve;
  int y, yAve;
  long xAveBuff, yAveBuff;
  
  // Read compass values
  compass.read();

  byte a = compass.getAzimuth();

  char myArray[3];
  compass.getDirection(myArray, a);

  // Return XYZ readings
  x = compass.getX();
  y = compass.getY();

  AveragingXArray[AveragingLoop] = x;
  AveragingYArray[AveragingLoop] = y;
  AveragingLoop++;
  if (AveragingLoop >= ArraySize) {
    AveragingLoop = 0;
  }

 
  xAveBuff = 0;
  for (int i = 0; i < ArraySize; i++) {
    xAveBuff += AveragingXArray[i];
  }
  xAve = xAveBuff / ArraySize / 10;
  
  yAveBuff = 0;
  for (int j = 0; j < ArraySize; j++) {
    yAveBuff += AveragingYArray[j];
  }
  yAve = yAveBuff / ArraySize / 10;


  Serial.print("xRaw: ");
  Serial.print(x);
  Serial.print(" xAve: ");
  Serial.print(xAve);

  Serial.print(" yRaw: ");
  Serial.print(y);
  Serial.print(" yAve: ");
  Serial.print(yAve);

  Serial.println();
  Serial.print(myArray[0]);
  Serial.print(myArray[1]);
  Serial.print(myArray[2]);
  Serial.println();
  Serial.println();
  
  delay(50);
}
