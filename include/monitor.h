#pragma once //prevent file from being included more than once
#include <Arduino.h>

class MON
{
private:

public:
    void setup();
    void stop();
    static void IRAM_ATTR MONloop(void *pvParameters);
    void printMemoryUsage();
};

extern MON mon;