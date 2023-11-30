#include <chrono>
#include <map>
#include <fft.h>
#include <algorithm>
#include <iterator>
using namespace std;
using namespace std::chrono;


arduinoFFT FFT = arduinoFFT();

const double samplingFrequency = 3600;
const uint16_t samples = 2048;
double vReal[samples];
double vImag[samples];
double fftSave[2][samples];

void fftPrint(double arr[2][samples]) {
   Serial.print("Freq: ");
   for (int i = 0; i < samples ; i++) {
      Serial.print(arr[0][i]); Serial.print(" ");
   }
   Serial.println(" ");
   Serial.println(" ");
   Serial.print("Mag: ");
   for (int i = 0; i < samples ; i++) {
      Serial.print(arr[1][i]); Serial.print(" ");
   }
}

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

   if(scaleType==SCL_FREQUENCY) {
      fftSave[0][i] = abscissa;
   }
   }
   Serial.println();
}

// https://forum.arduino.cc/t/using-arduinofft-with-an-accelerometer-to-detect-vibration-freq/609323/8

void fft(double vReal[2048]) {
   FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
   FFT.Compute(vReal, vImag, samples, FFT_FORWARD); //Compute FFT
   FFT.ComplexToMagnitude(vReal, vImag, samples); // Compute magnitudes

   std::copy(vReal, vReal+2048, fftSave[1]);
   PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
   fftPrint(fftSave);
}