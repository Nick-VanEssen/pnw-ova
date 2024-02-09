#pragma once

class micData{
private:
public:
    double micFFTData[1024];
    bool micFlag;
};

class accData{
private:
public:
    double accFFTData[1024];
    bool accFlag;
};

extern accData accdata;
extern micData micdata;