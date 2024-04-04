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
using namespace std;
using namespace std::chrono;

// http://wiki.openmusiclabs.com/wiki/ArduinoFFT

arduinoFFT FFTfunc = arduinoFFT();

const uint16_t samples = 2048;
// double copiedArr[samples];
double vImag[samples];
double freq[samples / 2];
double mag[samples / 2];
micData micdata;
accData accdata;
fstream fout; 

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

void saveValues(double vData[2048], double samplingFrequency) {
   if(samplingFrequency == 16000) {
      std::fill_n(micdata.micFFTData, samples/ 2, 0);
      std::copy(vData, vData+1024, micdata.micFFTData);
      // Serial.println(" \n");
      // Serial.print("MIC FFT DATA");
      for (int i = 0; i < samples / 2; i++)
   {
      // Serial.print(micdata.micFFTData[i] / samples, 6);
      // Serial.print(" ");
   }
   }
   else {
      Serial.println(" \n");
      Serial.print("ACC FFT DATA");
      std::fill_n(accdata.accFFTData, samples/ 2, 0);
      std::copy(vData, vData+1024, accdata.accFFTData);
      for (int i = 0; i < samples / 2; i++)
   {
      Serial.print(accdata.accFFTData[i] / samples, 6);
      Serial.print(", ");
   }
   else
   {
      // Serial.println(" \n");
      // Serial.print("ACC FFT DATA");
      std::fill_n(accdata.accFFTData, samples / 2, 0);
      std::copy(vData, vData + 1024, accdata.accFFTData);
#ifdef PRINT_DATA
      for (int i = 0; i < samples / 2; i++)
      {
         // Serial.print(accdata.accFFTData[i] / samples, 6);
         // Serial.print(" ");
      }
#endif
   }
}



// https://forum.arduino.cc/t/using-arduinofft-with-an-accelerometer-to-detect-vibration-freq/609323/8

void calc(double vReal[2048], double samplingFrequency)
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

   std::fill_n(vImag, samples, 0);
   std::fill_n(vReal, samples, 0);
}

// void WebSocketLog(double data)
// {
//    // Serial.println("Log websocket.");
//    // Serial.printf("%d clients connected.\n", GetClientsCount());

//    for (uint8_t index = 0; index < maxClients; index++)
//    {
//       WebsocketsClient *client = _clients[index];
//       if (client == NULL)
//       {
//          continue;
//       }

//       if (!client->available() || !client->send(data))
//       {
//          _clients[index] = NULL;
//          // Serial.println("Remove disconnected websocket client from Log().");
//          client->close();
//          delete client;
//       }
//    }
// }

// Websocket functions
// void getFrequencyData(double *outArray, size_t length)
// {
//    for (size_t i = 0; i < length && i < samples / 2; i++)
//    {
//       Serial.println(" ");
//       Serial.println("************************ARRAY DATA NEEDED************************: ");
//       outArray[i] = accdata[i];
//       Serial.print(accdata[i]);
//    }
// }

// void getMagnitudeData(double *outArray, size_t length)
// {
//    for (size_t i = 0; i < length && i < samples / 2; i++)
//    {
//       outArray[i] = mag[i];
//    }
// }
