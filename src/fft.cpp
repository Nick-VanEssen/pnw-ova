#include <Arduino.h>
#include <FFT_signal.h>
using namespace std;

void fft(double xval, double yval, double zval) {
    // DO FFT
    
    std::map<double, double> xmap;
    std::map<double, double> ymap;
    std::map<double, double> zmap;

    &fft_signal.push_back(xval);

    // DETECT WHAT STAGE
    // DISPLAY ON GRAPH IN FRONT END
    // STORE DATA IN MAP TO SAVE FOR LATER (may need linkedhashmap due to map not keeping order)
    // ADD MORE IF HAVE TO
}