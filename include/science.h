#pragma once

#include <Arduino.h>

class Measurement
{
private:
    int value;
    unsigned long timestamp;

public:
    void setValue(int val)
    {
        value = val;
        this->timestamp = millis();
    }
    int getValue() { return value; }

    long getTimestamp() { return timestamp; }

    Measurement()
    {
        this->value = 0;
        this->timestamp = millis();
    }
};