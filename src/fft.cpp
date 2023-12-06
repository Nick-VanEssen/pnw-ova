#include <chrono>
#include <map>
#include <fft.h>
#include <algorithm>
#include <iterator>
#include <settings.h>
#include <main.h>
using namespace std;
using namespace std::chrono;

// http://wiki.openmusiclabs.com/wiki/ArduinoFFT

arduinoFFT FFTfunc;

const double samplingFrequency = 3600;
const uint16_t samples = 2048;
double vImag[samples];
double freq[samples/2];
double mag[samples/2];

void fftPrint() {
   Serial.print("Freq: ");
   for (int i = 0; i < samples/2 ; i++) {
      Serial.print(freq[i]); Serial.print(" ");
   }
   Serial.println(" ");
   Serial.println(" ");
   Serial.print("Mag: ");
   for (int i = 0; i < samples/2 ; i++) {
      Serial.print(mag[i]); Serial.print(" ");
   }
}

void logFreq(double vData[2048], uint16_t bufferSize)
{
   for (uint16_t i = 0; i < bufferSize; i++)
      {
         freq[i] = ((i * 1.0 * samplingFrequency) / samples);
         mag[i] = vData[i];
      }
}

// https://forum.arduino.cc/t/using-arduinofft-with-an-accelerometer-to-detect-vibration-freq/609323/8

void calc(double vReal[2048]) {
   auto stop = high_resolution_clock::now();
   duration<double> time_span = duration_cast<duration<double>>(stop - getStartTime());
   Serial.print("Time: "); Serial.print(time_span.count()); Serial.print(" sec/ "); //should be inerval of .6 seconds

   FFTfunc = arduinoFFT(vReal, vImag, samples, samplingFrequency); /* Create FFT object */
   FFTfunc.Compute(FFT_FORWARD); //Compute FFT
   FFTfunc.ComplexToMagnitude(); // Compute magnitudes

   logFreq(vReal,samples/2);
   fftPrint();
}