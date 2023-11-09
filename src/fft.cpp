#include <Arduino.h>
// #include <fftw.h>
#include <chrono>
#include <map>
using namespace std;
using namespace std::chrono;

void fft(double xval, double yval, double zval, long time) {
    // CREATE MAP AND STORE ACCELEROMETER VALUES        
    std::map<long, double> xmap;
    std::map<long, double> ymap;
    std::map<long, double> zmap;

    xmap.insert(pair<long, double>(time, xval));
    ymap.insert(pair<long, double>(time, yval));
    zmap.insert(pair<long, double>(time, zval));

    // CREATE 2D ARRAY TO INPUT INTO FFT FUNCTION
    double val = xval + yval + zval;
    


    /* 
    https://www.fftw.org/fftw3_doc/Complex-One_002dDimensional-DFTs.html
    FFTW3 is a great library that is popular and will work

    https://stackoverflow.com/questions/5685765/computing-fft-and-ifft-with-fftw-library-c
    GOOD EXAMPLE
    
     question, we need a 2D array to do fftw, meaning we need to take a 
     sample -> do fft -> then store/ send to graph -> repeat
     we need to decide how often we want to update 1s?
     */


    /* ******* WILL DO LATER IN ANOTHER TASK *************
    DETECT WHAT STAGE OR ALERT BAD
    DISPLAY ON GRAPH IN FRONT END
    STORE DATA IN MAP TO SAVE FOR LATER (may need linkedhashmap due to map not keeping order)
    ADD MORE IF HAVE TO
    */
}