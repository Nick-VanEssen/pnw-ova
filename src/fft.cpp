#include <Arduino.h>
#include <chrono>
#include <map>
using namespace std;
using namespace std::chrono;

void fft(double arr[2048][2], long time) {
    
    /* 
    https://www.fftw.org/fftw3_doc/Complex-One_002dDimensional-DFTs.html
    FFTW3 is a great library that is popular and will work
    
    https://stackoverflow.com/questions/5685765/computing-fft-and-ifft-with-fftw-library-c
    GOOD EXAMPLE
    
    MAY SWITCH TO ARDUINOFFT
    https://registry.platformio.org/libraries/kosme/arduinoFFT
    EXAMPLE
    https://registry.platformio.org/libraries/kosme/arduinoFFT/examples/FFT_01/FFT_01.ino
    
     break out fft into a new task so we can use for MIC
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