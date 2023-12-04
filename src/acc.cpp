#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <map>
#include <acc.h>
#include <main.h>
#include <fft.h>
#include <settings.h>

/*Initialize an instance of Adafruit_ADXL345_Unified with a unique id*/
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
double arr2[2048];
int i = 0;
TaskHandle_t ACCTask;

ACC acc;

void ACC::setup() {  
  accel.begin();

  xTaskCreatePinnedToCore(ACCloop,           /* Task function. */
                          "ACCTask",         /* name of task. */
                          ACC_STACK_SIZE,    /* Stack size of task*/
                          NULL,              /* parameter of the task */
                          ACC_TASK_PRIORITY, /* priority of             /* priority of the task*/
                          &ACCTask,          /* Task handle to keep track of created task */
                          ACC_TASK_CORE);    /* pin task to core 0 */
  Serial.printf("ACC task started");
}

void store(double xval, double yval, double zval, long time) {
    // CREATE MAP AND STORE ACCELEROMETER VALUES        
    // std::map<long, double> xmap;
    // std::map<long, double> ymap;
    // std::map<long, double> zmap;

    // xmap.insert(pair<long, double>(time, xval));
    // ymap.insert(pair<long, double>(time, yval));
    // zmap.insert(pair<long, double>(time, zval));

    // CREATE 2D ARRAY TO INPUT INTO FFT FUNCTION
    double val = xval + yval + zval;
    arr2[i] = val;
    std::copy(arr2, arr2+2048, fft.vReal);
    fft.setup();
}

void ACC::ACCloop(void *pvParameters) {
  for(i = 0; i <2048; i++) {
    Serial.println("");
    /*Read from ADXL345 accelerometer*/
    sensors_event_t event;
    accel.getEvent(&event);
    double xval = (event.acceleration.x);
    double yval = (event.acceleration.y);
    double zval = (event.acceleration.z)-9.81; //will need to change what variable is affected by gravity depending on board orientation

    // Serial.print("X: "); Serial.print(xval); Serial.print("  ");
    // Serial.print("Y: "); Serial.print(yval); Serial.print("  ");
    // Serial.print("Z: "); Serial.print(zval); Serial.print("  ");
    
    auto stop = high_resolution_clock::now();
  
    duration<double> time_span = duration_cast<duration<double>>(stop - getStartTime());
    auto milliseconds = chrono::duration_cast< std::chrono::milliseconds >( time_span );
    // Serial.print("Time: "); Serial.print(time_span.count()); Serial.print(" sec/ "); Serial.print(milliseconds.count()); Serial.print(" ms");
    store(xval,yval,zval, milliseconds.count());

   /*Take a 0.3125 ms break*/
    delay(0.3125);
  }
}

void ACC::printMemoryUsage()
{
  Serial.printf("ACC FREE STACK: %d \n", uxTaskGetStackHighWaterMark(ACCTask));
}

void ACC::stop()
{
  Serial.println("Stopping ACC Task");
  if (ACCTask != NULL)
  {
    vTaskDelete(ACCTask);
    ACCTask = NULL;
  }
}