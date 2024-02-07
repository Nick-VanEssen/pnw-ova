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

class Baseline{
private:
public:
    double accBaseline[1024];
    double micBaseline[1024];
};

extern accData accdata;
extern micData micdata;
extern Baseline base;