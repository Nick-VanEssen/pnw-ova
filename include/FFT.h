#pragma once //prevent file from being included more than once
#include <Arduino.h>
#include <arduinoFFT.h>


    #define SCL_INDEX 0x00
    #define SCL_TIME 0x01
    #define SCL_FREQUENCY 0x02
    #define SCL_PLOT 0x03

    void calc(double vReal[2048], double samplingFrequency);
    

