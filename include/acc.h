#pragma once //prevent file from being included more than once
#include <Arduino.h>

class ACC
{
private:

public:
    void setup();
    static void ACCloop();
    // void printMemoryUsage();
};

extern ACC acc;