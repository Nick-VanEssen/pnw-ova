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

extern accData accdata;
extern micData micdata;