#include <chrono>
#include <map>
#include <fft.h>
#include <algorithm>
#include <iterator>
#include <settings.h>
using namespace std;
using namespace std::chrono;


TaskHandle_t FFTTask;
FFT fft;
arduinoFFT FFTfunc;

const double samplingFrequency = 3600;
const uint16_t samples = 2048;
double vReal[samples];
double vImag[samples];
double freq[samples];

void FFT::setup() {  
   xTaskCreatePinnedToCore(calc,           /* Task function. */
                          "FFTTask",         /* name of task. */
                          FFT_STACK_SIZE,    /* Stack size of task*/
                          NULL,              /* parameter of the task */
                          FFT_TASK_PRIORITY, /* priority of             /* priority of the task*/
                          &FFTTask,          /* Task handle to keep track of created task */
                          FFT_TASK_CORE);    /* pin task to core 0 */
  Serial.printf("FFT task started");
  stopFlag = false;
}

void fftPrint() {
   Serial.print("Freq: ");
   for (int i = 0; i < samples/2 ; i++) {
      Serial.print(freq[i]); Serial.print(" ");
   }
   Serial.println(" ");
   Serial.println(" ");
   Serial.print("Mag: ");
   for (int i = 0; i < samples/2 ; i++) {
      Serial.print(fft.vReal[i]/samples); Serial.print(" ");
   }
}

void logFreq(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
   for (uint16_t i = 0; i < bufferSize; i++)
   {
      double abscissa;
      /* Print abscissa value */
      switch (scaleType)
      {
         case SCL_INDEX:
         abscissa = (i * 1.0);
      break;
      case SCL_TIME:
         abscissa = ((i * 1.0) / samplingFrequency);
      break;
      case SCL_FREQUENCY:
         abscissa = ((i * 1.0 * samplingFrequency) / samples);
      break;
   }

   if(scaleType==SCL_FREQUENCY) {
      freq[i] = abscissa;
   }
   }
   Serial.println();
}

// https://forum.arduino.cc/t/using-arduinofft-with-an-accelerometer-to-detect-vibration-freq/609323/8

void calc(void *pvParameters) {
   FFTfunc = arduinoFFT(vReal, vImag, samples, samplingFrequency); /* Create FFT object */
   FFTfunc.Compute(FFT_FORWARD); //Compute FFT
   FFTfunc.ComplexToMagnitude(); // Compute magnitudes

   logFreq(vReal, samples/2, SCL_FREQUENCY);
   fftPrint();
   fft.stopFlag = true;
   while(true) {
      vTaskDelay(100);
   }
}


void FFT::stop()
{
  Serial.println("Stopping FFT Task");
  if (FFTTask != NULL)
  {
    vTaskDelete(FFTTask);
    FFTTask = NULL;
  }
}