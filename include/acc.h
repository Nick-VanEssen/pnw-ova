#pragma once //prevent file from being included more than once
#include <Arduino.h>

class ACC
{
private:

public:
    void setup();
    void stop();
    static void IRAM_ATTR ACCloop(void *pvParameters);
    void printMemoryUsage();
};

extern ACC acc;