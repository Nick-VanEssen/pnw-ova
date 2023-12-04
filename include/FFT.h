#pragma once //prevent file from being included more than once
#include <Arduino.h>
#include <arduinoFFT.h>

class FFT
{
private:
    #define SCL_INDEX 0x00
    #define SCL_TIME 0x01
    #define SCL_FREQUENCY 0x02
    #define SCL_PLOT 0x03
public:
    double vReal[2048];
    void setup();
    bool stopFlag;
    void stop();
};

static void calc(void *pvParameters);

extern FFT fft;