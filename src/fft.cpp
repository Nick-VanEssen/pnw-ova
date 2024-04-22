#include <chrono>
// #include <map>
#include <fft.h>
#include <algorithm>
#include <iterator>
#include <settings.h>
#include <main.h>
#include <global.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <email.h>
using namespace std;
using namespace std::chrono;

// http://wiki.openmusiclabs.com/wiki/ArduinoFFT

arduinoFFT FFTfunc = arduinoFFT();
stopStartToggle stopstarttoggle;
bool badDataFlag = 0;
int alertCounter = 0;
int averagedDataSets = 0;
bool beginAverage = 1;
bool hardCodedAlgorithmFlag = 1;
goodAccData goodaccdata;
const uint16_t samples = 2048;
// double copiedArr[samples];
double vImag[samples];
double freq[samples / 2];
double mag[samples / 2];
micData micdata;
accData accdata;
fstream fout;
MAILRESULTS mailResults;

void fftPrint(double vReal[2048])
{
   Serial.println(" ");
   Serial.print("Freq: ");
   for (int i = 0; i < samples / 2; i++)
   {
      Serial.print(freq[i]);
      Serial.print(" ");
   }
   Serial.print("Original: ");
   for (int i = 0; i < samples; i++)
   {
      Serial.print(vReal[i]);
      Serial.print(" ");
   }
   Serial.println(" ");
   Serial.println(" ");
   Serial.print("Mag: ");
   for (int i = 0; i < samples / 2; i++)
   {
      Serial.print(mag[i] / samples, 6);
      Serial.print(" ");
   }
   std::fill_n(freq, samples / 2, 0);
   std::fill_n(mag, samples / 2, 0);
}

void logFreq(double vData[2048], uint16_t bufferSize, double samplingFrequency)
{
   for (uint16_t i = 0; i < bufferSize; i++)
   {
      freq[i] = ((i * 1.0 * samplingFrequency) / samples);
      mag[i] = vData[i];
   }
}

void emailNotification()
{
   if (averagedDataSets < 50)
   {
      for (int i = 0; i < 1024; i++)
      {
         if ((accdata.accFFTData[i]) > .1)
         {
            Serial.print("\n");
            Serial.print(accdata.accFFTData[i]);
            Serial.print("Out of range!");
            Serial.print("\n");
            beginAverage = 0;
            break;
         }
      }
      if (beginAverage == 1)
      {
         Serial.print("\n");
         Serial.print("Averaging...");
         for (int i = 0; i < 1024; i++)
         {
            goodaccdata.goodData[i] += (accdata.accFFTData[i]);
         }
         averagedDataSets++;
      }
      Serial.print("\n");
      Serial.print(averagedDataSets);
      Serial.print("\n");
      beginAverage = 1;
   }
   if (averagedDataSets == 50)
   {
      for (int i = 0; i < 1024; i++)
      {
         // Serial.print(goodaccdata.goodData[i] / 50, 6);
         // Serial.print(", ");
         // || ((accdata.accFFTData[i]) < ((goodaccdata.goodData[i]) * 0.00001))
         if (((accdata.accFFTData[i]) > ((goodaccdata.goodData[i] / 50) * 5)))
         {
            badDataFlag = 1;
            Serial.print("\n");
            Serial.print("Bad Data Detected! Data Range Expected: ");
            Serial.print(goodaccdata.goodData[i] / 50, 6);
            Serial.print("-");
            Serial.print((goodaccdata.goodData[i] / 50) * 5, 6);
            Serial.print(" -----> ");
            Serial.print("Data Received: ");
            Serial.print(accdata.accFFTData[i], 6);
            Serial.print(" at array index: ");
            Serial.print(i);
         }
      }

      if (badDataFlag == 0)
      {
         Serial.print("\n");
         Serial.print("-------------Data passed check--------------");
         Serial.print("\n");
         alertCounter = 0;
      }
      if (badDataFlag == 1)
      {
         alertCounter++;
         Serial.print("\n");
         Serial.print("Alert counter at: ");
         Serial.print(alertCounter);
         Serial.print("/5");
         Serial.print("\n");
         if (alertCounter == 5)
         {
            Serial.print("\n");
            Serial.print("!-!-!-!-!-!-!-!-DATA FAILED CHECK, SENDING ALERT-!-!-!-!-!-!-!-!");
            Serial.print("\n");
            // mailResults.send();
         }
         badDataFlag = 0;
      }
   }
}

void saveValues(double vData[2048], double samplingFrequency)
{
   // if (samplingFrequency == 16000)
   // {
   //    std::fill_n(micdata.micFFTData, samples / 2, 0);
   //    std::copy(vData, vData + 1024, micdata.micFFTData);
   //    // Serial.println(" \n");
   //    // Serial.print("MIC FFT DATA");                      for pdm mic we do not need
   //    for (int i = 0; i < samples / 2; i++)
   //    {
   //       // Serial.print(micdata.micFFTData[i] / samples, 6);
   //       // Serial.print(" ");
   //    }
   // }
   // else
   // {
   Serial.println(" \n");
   std::fill_n(accdata.accFFTData, samples / 2, 0);
   for (int i = 0; i < samples / 2; i++)
   {
      accdata.accFFTData[i] = vData[i] / samples;
   }

   if (hardCodedAlgorithmFlag == 1)
   {
      emailNotification();
   }
   // }
}

void printValues()
{
   for (int i = 0; i < samples / 2; i++)
   {
      Serial.print(accdata.accFFTData[i], 6);
      Serial.print(", ");
   }
}

// https://forum.arduino.cc/t/using-arduinofft-with-an-accelerometer-to-detect-vibration-freq/609323/8

void calc(double vReal[2048], double samplingFrequency)
{
   if (stopstarttoggle.stopStartFlag == 1)
   {

      // std::copy(vReal, vReal+2048, copiedArr);
      // auto stop = high_resolution_clock::now();
      // duration<double> time_span = duration_cast<duration<double>>(stop - getStartTime());
      // Serial.print("Time: "); Serial.print(time_span.count()); Serial.print(" sec/ "); //should be interval of .6 seconds

      FFTfunc.DCRemoval(vReal, samples);
      FFTfunc.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFTfunc.Compute(vReal, vImag, samples, FFT_FORWARD); // Compute FFT
      FFTfunc.ComplexToMagnitude(vReal, vImag, samples);   // Compute magnitudes

      // logFreq(vReal, samples / 2, samplingFrequency);
      // fftPrint(vReal);
      saveValues(vReal, samplingFrequency);
      // printValues();

      std::fill_n(vImag, samples, 0);
      std::fill_n(vReal, samples, 0);
   }
}