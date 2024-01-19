#include <monitor.h>
#include <settings.h>
#include <global.h>

TaskHandle_t MONTask;
MON mon;

void MON::setup()
{
  xTaskCreatePinnedToCore(MONloop,           /* Task function. */
                          "MONTask",         /* name of task. */
                          MON_STACK_SIZE,    /* Stack size of task*/
                          NULL,              /* parameter of the task */
                          MON_TASK_PRIORITY, /* priority of             /* priority of the task*/
                          &MONTask,          /* Task handle to keep track of created task */
                          MON_TASK_CORE);    /* pin task to core 0 */
  Serial.printf("MON task started");
}

void MON::MONloop(void *pvParameters)
{
  // 1 or 2 monitoring tasks?
  // check acc fft data
  accFlag = true;

  // check mic fft data
  micFlag = true;
}