#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <map>
#include <fft.cpp>
using namespace std;

/*Initialize an instance of Adafruit_ADXL345_Unified with a unique id*/
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
 
void setup() {
 
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Small example to read from ADXL345 accelerometer");
  accel.begin();
 
  std::map<double, double> xmap;
  std::map<double, double> ymap;
  std::map<double, double> zmap;

  time_t start, end;
  time(&start);
}

void send(double xval, double yval, double zval) {
    fft(xval, yval, zval);
}

void loop() {
    
  int i = 0;;
  for(i;i=2047;i++) {
    Serial.println("");
    /*Read from ADXL345 accelerometer*/
    sensors_event_t event; 
    accel.getEvent(&event);
    double xval = (event.acceleration.x);
    double yval = (event.acceleration.y);
    double zval = (event.acceleration.z)/9.81;

    Serial.print("X: "); Serial.print(xval); Serial.print("  ");
    Serial.print("Y: "); Serial.print(yval); Serial.print("  ");
    Serial.print("Z: "); Serial.print(zval); Serial.print("  ");
    //ADD X Y Z VALUE TO RESPECTIVE MAPS
    send(xval,yval,zval);
    }
    //clear maps
    i = 0;
  // /*Take a 0.1 second break*/
    delay(100);

}