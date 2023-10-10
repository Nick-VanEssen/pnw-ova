These programs will allow you to test the functionality of the board, currently we have separate programs to test the components individually. 
In the future we will create programs that can test both components and perform computations such as FFT and filtering.



For the accelerometer testing code, you will need to install a few libraries on Arduino IDE.
First, you need to install the "Adafruit Unifier Sensor" library by Adafruit.
Next, install the "Adafruit ADXL345" library by Adafruit.
The ESP32 board can be on any version in the boards manager tab.
These are all you will need to test the accelerometer, but for more information consult this article: https://www.techcoil.com/blog/how-to-use-a-esp32-development-board-to-read-from-an-adxl345-accelerometer/



For the microphone testing code, you do not need to install any libraries.
The ESP32 board needs to be on version 1.0.3 in the boards manager tab for the code to work. 
This program has very basic filtering, but it still needs a lot of work.
This article contains more information: https://diyi0t.com/i2s-sound-tutorial-for-esp32/
