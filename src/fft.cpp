#include <chrono>
#include <map>
#include <fft.h>
using namespace std;
using namespace std::chrono;

arduinoFFT FFT = arduinoFFT();

const double samplingFrequency = 3600;
const uint16_t samples = 2048;
double vReal[samples];
double vImag[samples];

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
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
   Serial.print(abscissa, 6);
   if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
      Serial.print(" ");
      Serial.println(vData[i], 4);
   }
   Serial.println();
}

// https://forum.arduino.cc/t/using-arduinofft-with-an-accelerometer-to-detect-vibration-freq/609323/8

void fft(double vReal[2048]) {
   FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
   FFT.Compute(vReal, vImag, samples, FFT_FORWARD); //Compute FFT
   FFT.ComplexToMagnitude(vReal, vImag, samples); // Compute magnitudes

   Serial.println("Computed magnitudes:");
   PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
   double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
   Serial.println(x, 6);
}