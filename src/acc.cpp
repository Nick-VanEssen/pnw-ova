#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
// #include <map>
#include <acc.h>
#include <main.h>
#include <fft.h>
#include <global.h>
#include <settings.h>

/*Initialize an instance of Adafruit_ADXL345_Unified with a unique id*/
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
int i = 0;
TaskHandle_t ACCTask;
bool flag1;
bool accFlag = false;
SemaphoreHandle_t xSemaphore = NULL;
ACC acc;
double val, xval, yval, zval;
double arr[2048];
void print(double arr[2048]);
sensors_event_t event;

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
  accel.setDataRate(ADXL345_DATARATE_3200_HZ);
  xSemaphore = xSemaphoreCreateMutex();
}

void ACC::ACCloop(void *pvParameters)
{
  while (true)
  {
    if (xSemaphore != NULL)
    {
      if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE)
      {
        for (i = 0; i < 2048; i++)
        {
          /*Read from ADXL345 accelerometer*/
          accel.getEvent(&event);
          xval = (event.acceleration.x);
          yval = (event.acceleration.y);
          zval = (event.acceleration.z);

          val = sqrt(pow(xval, 2) + pow(yval, 2) + pow(zval, 2));
          arr[i] = val;

          // Serial.print("X: "); Serial.print(xval); Serial.print("  ");
          // Serial.print("Y: "); Serial.print(yval); Serial.print("  ");         // used to print adxl345 data
          // Serial.print("Z: "); Serial.print(zval); Serial.print("  ");

          // auto stop = high_resolution_clock::now();
          // duration<double> time_span = duration_cast<duration<double>>(stop - getStartTime());           // Block is used to print time data taken
          // auto milliseconds = chrono::duration_cast< std::chrono::milliseconds >( time_span );
          // Serial.print("Time: "); Serial.print(time_span.count()); Serial.print(" sec/ "); Serial.print(milliseconds.count()); Serial.print(" ms");

          /*Take a 0.3125 ms break*/
          delay(ACC_SAMPLE_DELAY);
        }
        // print(arr);
        calc(arr, 3600.0);
        std::fill_n(arr, 2048, 0);
        xSemaphoreGive(xSemaphore);
      }
    }
    vTaskDelay(ACC_LOOP_DELAY);
  }
}

void print(double arr[2048])
{
  for (int i = 0; i < 2048; i++)
  {
    Serial.print(arr[i]);
    Serial.print(", ");
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