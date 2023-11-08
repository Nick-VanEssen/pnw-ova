#include <Arduino.h>
// #include <fftw.h>
#include <chrono>
#include <map>
using namespace std;
using namespace std::chrono;

void fft(double xval, double yval, double zval, int time) {
    // DO FFT
    
    std::map<int, double> xmap;
    std::map<int, double> ymap;
    std::map<int, double> zmap;

    xmap.insert(pair<int, double>(time, xval));
    ymap.insert(pair<int, double>(time, yval));
    zmap.insert(pair<int, double>(time, zval));

    double val = xval + yval + zval;
    
    
    // &fft_signal.push_back(xval);

    // DETECT WHAT STAGE
    // DISPLAY ON GRAPH IN FRONT END
    // STORE DATA IN MAP TO SAVE FOR LATER (may need linkedhashmap due to map not keeping order)
    // ADD MORE IF HAVE TO
}