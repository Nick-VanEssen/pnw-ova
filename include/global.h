#pragma once

#include "settings.h"

class micData
{
private:
public:
    double micFFTData[PDM_BUFFER_LEN];
    bool micFlag;
};

class accData
{
private:
public:
    double accFFTData[ACC_BUFFER_LEN];
    bool accFlag;
};

class goodAccData
{
private:
public:
    double goodData[1024];
};

class stopStartToggle
{
private:
public:
    bool stopStartFlag = 0;
};

class sensitivity
{
private:
public:
    double sensValue = 4.5;
};

class reAverageData
{
private:
public:
    bool reAverageFlag = 0;
};

extern accData accdata;
extern micData micdata;
extern goodAccData goodData;
extern stopStartToggle stopstarttoggle;
extern sensitivity sensitivityValue;
extern reAverageData reaveragedata;