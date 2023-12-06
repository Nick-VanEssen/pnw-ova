#include <Arduino.h>
// Include I2S driver
#include <driver/i2s.h>
#include "pdm_mic.h"

PDM pdm;

int16_t sBuffer[PDM_BUFFER_LEN];
TaskHandle_t PDMTask;
#ifdef DEBUG_PDM_LED
TaskHandle_t PDMDebug;
#endif

void PDM::setup()
{
  // Set up I2S
  i2s_install();
  i2s_setPin();
  i2s_start(I2S_PORT);
  //ledcSetup(uint8_t chan, uint32_t freq, uint8_t bit_num)
  ledcSetup(LED_CHANNEL, LED_FREQ, LEDC_TIMER_12_BIT);
  xTaskCreatePinnedToCore(PDMloop,           /* Task function. */
                          "PDMTask",         /* name of task. */
                          PDM_STACK_SIZE,    /* Stack size of task*/
                          NULL,              /* parameter of the task */
                          PDM_TASK_PRIORITY, /* priority of             /* priority of the task*/
                          &PDMTask,          /* Task handle to keep track of created task */
                          PDM_TASK_CORE);    /* pin task to core 0 */

  Serial.printf("PDM task started");

#ifdef DEBUG_PDM_LED
  xTaskCreatePinnedToCore(debugLED,                /* Task function. */
                          "PDMDebug",              /* name of task. */
                          PDM_DEBUG_STACK_SIZE,    /* Stack size of task*/
                          NULL,                    /* parameter of the task */
                          PDM_DEBUG_TASK_PRIORITY, /* priority of             /* priority of the task*/
                          &PDMDebug,               /* Task handle to keep track of created task */
                          PDM_DEBUG_TASK_CORE);    /* pin task to core 0 */

  Serial.printf("PDM Debug task started");
#endif
}

void PDM::PDMloop(void *pvParameters)
{
  for (;;)
  {

#ifdef DEBUG_PDM_SERIAL
    // False print statements to "lock range" on serial plotter display
    // Change rangelimit value to adjust "sensitivity"
    int rangelimit = PDM_DEBUG_RANGE_LIMIT;
    Serial.print(rangelimit * -1);
    Serial.print(" ");
    Serial.print(rangelimit);
    Serial.print(" ");
#endif

    // Get I2S data and place in data buffer
    size_t bytesIn = 0;
    esp_err_t result = i2s_read(I2S_PORT, &sBuffer, PDM_BUFFER_LEN, &bytesIn, portMAX_DELAY);

    if (result == ESP_OK)
    {
      // Read I2S data buffer
      int16_t samples_read = bytesIn / 8;
      if (samples_read > 0)
      {
        float mean = 0;
        for (int16_t i = 0; i < samples_read; ++i)
        {
          mean += (sBuffer[i]);
        }

        // Average the data reading
        mean /= samples_read;
        pdm.oStream.setValue(mean);

#ifdef DEBUG_PDM_SERIAL
        // Print to serial plotter
        Serial.println(mean);
#endif
      }
    }
  }
}

void PDM::i2s_install()
{
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
      .sample_rate = PDM_SAMPLE_RATE,
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_PCM_SHORT),
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = PDM_BUFFER_LEN,
      .use_apll = false,
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void PDM::i2s_setPin()
{
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = -1,
      .data_in_num = I2S_SD};

  i2s_set_pin(I2S_PORT, &pin_config);
}

#ifdef DEBUG_PDM_LED
// Vary the LED brightness as a single channel VU meter
void PDM::debugLED(void *pvParameters)
{
  float brightness = 0;
  for (;;)
  {
    if (pdm.oStream.getValue() > 0)
    {
      // need to cast to float for proper division
      brightness = (static_cast<float>(pdm.oStream.getValue()) / PDM_DEBUG_RANGE_LIMIT) * 255;
      ledcWrite(LED_CHANNEL, static_cast<uint32_t>(brightness));
    }
    // Some delay here helps to prevent flickering
    vTaskDelay(PDM_LED_DELAY);
  }
}
#endif

void PDM::printMemoryUsage()
{
  Serial.printf("PDM FREE STACK: %d \n", uxTaskGetStackHighWaterMark(PDMTask));

#ifdef DEBUG_PDM_LED
  Serial.printf("PDM LED FREE STACK: %d \n", uxTaskGetStackHighWaterMark(PDMDebug));
#endif
}

void PDM::stop()
{
  Serial.println("Stopping PDM Task");
  if (PDMTask != NULL)
  {
    vTaskDelete(PDMTask);
    PDMTask = NULL;
  }
}
