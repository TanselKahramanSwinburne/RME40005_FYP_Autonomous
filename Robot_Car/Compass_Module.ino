// Compass Module Code
// Author: Tansel A Kahraman
// Last Modified: 22/04/2020
// Description:
/*  Reads in value from a HMC5883L Compass.
 *  Convert the readings from the x and y axis into degree bearings to be used with an autonomous robot.
 *  The values read are already smoothed out, but then further averaged by the last 25 values for a smoother reading.
 */

// Procedure for setting up the compass module for communication.
void setupCompass() {
  
  compass.init();

  /*
   *   call setSmoothing(STEPS, ADVANCED);
   *   
   *   STEPS     = int   The number of steps to smooth the results by. Valid 1 to 10.
   *                     Higher steps equals more smoothing but longer process time.
   *                     
   *   ADVANCED  = bool  Turn advanced smoothing on or off. True will remove the max and min values from each step and then process as normal.
   *                     Turning this feature on will results in even more smoothing but will take longer to process.
   *                     
   */
  compass.setSmoothing(CompSmoothing,true);  
}

// Procedure for reading, averaging and converting values from the compass to degrees.
int readCompass() {
  int x, xAve;
  int y, yAve;
  int dirAngle;
  long xAveBuff, yAveBuff;

  // Read compass values.
  compass.read();

  // Return XY readings.
  x = compass.getX();
  y = compass.getY();

  // Store XY readings into averaging array.
  AveragingXArray[AveragingLoop] = x;
  AveragingYArray[AveragingLoop] = y;
  AveragingLoop++;
  if (AveragingLoop >= CompArraySize) AveragingLoop = 0;

  // Calculate the average x value. 
  xAveBuff = 0;
  for (int i = 0; i < CompArraySize; i++) {
    xAveBuff += AveragingXArray[i];
  }
  xAve = xAveBuff / CompArraySize;

  // Calculate the average y value.
  yAveBuff = 0;
  for (int j = 0; j < CompArraySize; j++) {
    yAveBuff += AveragingYArray[j];
  }
  yAve = yAveBuff / CompArraySize;

  // Calculate the direction in degrees.
  dirAngle = atan2(xAve, yAve)/0.0174532925;
  if (dirAngle < 0) dirAngle+=360;
  if (dirAngle >= 360) dirAngle-=360;

  return dirAngle;
}
