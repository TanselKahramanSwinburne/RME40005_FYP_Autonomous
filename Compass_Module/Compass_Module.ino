/*
===============================================================================================================
QMC5883LCompass.h Library XYZ Example Sketch
Learn more at [https://github.com/mprograms/QMC5883Compas]

This example shows how to use smoothing to get more stable readings.

===============================================================================================================
Release under the GNU General Public License v3
[https://www.gnu.org/licenses/gpl-3.0.en.html]
===============================================================================================================
*/
#include <QMC5883LCompass.h>

#define ARRAY_SIZE 10

QMC5883LCompass compass;
int averagingXArray[ARRAY_SIZE] = {0,0,0,0,0,0,0,0,0,0};
int averagingLoop = 0;

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
  int x, xAve, xAveBuff;
  
  // Read compass values
  compass.read();

  // Return XYZ readings
  x = compass.getX();

  averagingXArray[averagingLoop] = x;
  averagingLoop++;
  if (averagingLoop >= ARRAY_SIZE + 1) {
    averagingLoop = 0;
  }
  
  xAveBuff = 0;
  for (int i = 0; i <= ARRAY_SIZE; i++) {
    xAveBuff = xAveBuff + averagingXArray[i];
  }
  
  xAve = 0;
  xAve = xAveBuff / (ARRAY_SIZE + 1);


  
  
  Serial.print("L: ");
  Serial.print(averagingLoop);
  Serial.print(" X: ");
  Serial.print(x);
  Serial.println();

  Serial.print("X ARRAY = [ ");
  for (int i = 0; i <= ARRAY_SIZE; i++) {
    Serial.print(averagingXArray[i]);
    Serial.print(" ");
  }
  Serial.print("]");
  Serial.println();

  Serial.print("Xave: ");
  Serial.print(xAve);
  Serial.println();
  Serial.println();
  
  delay(50);
}
