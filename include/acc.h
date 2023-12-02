#pragma once //prevent file from being included more than once
#include <Arduino.h>

class ACC
{
private:
    void i2s_setPin();
    void i2s_install();
    static void IRAM_ATTR PDMloop(void *pvParameters);
    
public:
    void setup();
    void loop();
    // void printMemoryUsage();
};

extern ACC acc;