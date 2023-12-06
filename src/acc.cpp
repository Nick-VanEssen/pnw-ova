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
int i = 0;
TaskHandle_t ACCTask;
bool flag1;

ACC acc;

void ACC::setup()
{
  accel.begin();
  // memset(arr, 0, sizeof(arr));
  xTaskCreatePinnedToCore(ACCloop,           /* Task function. */
                          "ACCTask",         /* name of task. */
                          ACC_STACK_SIZE,    /* Stack size of task*/
                          NULL,              /* parameter of the task */
                          ACC_TASK_PRIORITY, /* priority of             /* priority of the task*/
                          &ACCTask,          /* Task handle to keep track of created task */
                          ACC_TASK_CORE);    /* pin task to core 0 */
  Serial.printf("ACC task started");
}

void ACC::ACCloop(void *pvParameters)
{
  double val;
  double xval;
  double yval;
  double zval;
  double arr[2048];
  while (true)
  {
    for (i = 0; i < 2048; i++)
    {
      /*Read from ADXL345 accelerometer*/
      sensors_event_t event;
      accel.getEvent(&event);
      xval = (event.acceleration.x);
      yval = (event.acceleration.y);
      zval = (event.acceleration.z) - 9.81; // will need to change what variable is affected by gravity depending on board orientation

      // Serial.print("X: "); Serial.print(xval); Serial.print("  ");
      // Serial.print("Y: "); Serial.print(yval); Serial.print("  ");         // used to print adxl345 data
      // Serial.print("Z: "); Serial.print(zval); Serial.print("  ");

      // auto stop = high_resolution_clock::now();
      // duration<double> time_span = duration_cast<duration<double>>(stop - getStartTime());           // Block is used to print time data taken
      // auto milliseconds = chrono::duration_cast< std::chrono::milliseconds >( time_span );
      // Serial.print("Time: "); Serial.print(time_span.count()); Serial.print(" sec/ "); Serial.print(milliseconds.count()); Serial.print(" ms");

      val = xval + yval + zval;
      arr[i] = (double)val;
      /*Take a 0.3125 ms break*/
      delay(0.3125);
    }
    calc(arr);
    vTaskDelay(1000);
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