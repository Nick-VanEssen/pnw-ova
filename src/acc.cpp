#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <acc.h>

/*Initialize an instance of Adafruit_ADXL345_Unified with a unique id*/

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
 
void setupAcc() {
 
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Small example to read from ADXL345 accelerometer");
  accel.begin();
 
}
 
void loopAcc() {
 Serial.println("");
  /*Read from ADXL345 accelerometer*/
  sensors_event_t event; 
  accel.getEvent(&event);
  // Serial.println("Printing XYZ: ");
  // Serial.println("");
  // /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  // /*Take a one second break*/
  delay(100);

 
}