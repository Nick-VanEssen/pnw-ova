#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <map>
#include <fft.cpp>
#include <chrono>
using namespace std;
using namespace std::chrono;

/*Initialize an instance of Adafruit_ADXL345_Unified with a unique id*/
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
 
void accSetup() {
  Serial.println("");
  Serial.println("Small example to read from ADXL345 accelerometer");
  
  accel.begin();
  auto start = high_resolution_clock::now();

  // time_t start, end;
  // time(&start);
}

 high_resolution_clock::time_point startClock() {
  auto start = high_resolution_clock::now();
  return start;
}

void send(double xval, double yval, double zval, long time) {
    fft(xval, yval, zval, time);
}

void accLoop() {
    
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

    send(xval,yval,zval, time_t);
    }
    //clear maps
    i = 0;
  // /*Take a 0.1 second break*/
    delay(100);

}