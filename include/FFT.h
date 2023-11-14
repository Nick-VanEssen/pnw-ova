#pragma once //prevent file from being included more than once
#include <Arduino.h>
#include <arduinoFFT.h>

/* *************** DECLARE FUNCTIONS HERE *************** */
//void fft(double arr[2048][2], long time);
void fft(double arr[2048]);

/* *************** VARIABLES FUNCTIONS HERE *************** */
arduinoFFT FFT = arduinoFFT();

const double samplingFrequency = 3600;
const uint16_t samples = 2048;
double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03