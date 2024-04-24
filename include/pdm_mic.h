#pragma once
#include "science.h"
#include "settings.h"

class PDM
{
private:
    void i2s_setPin();
    void i2s_install();
    static void IRAM_ATTR PDMloop(void *pvParameters);
#ifdef DEBUG_PDM_LED
    static void IRAM_ATTR debugLED(void *pvParameters);
#endif

public:
    Measurement oStream; // Output Stream

    void setup();
    void stop();
    void printMemoryUsage();
};

extern PDM pdm;
