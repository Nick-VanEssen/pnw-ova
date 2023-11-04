#pragma once


void setupPDM();
void loopPDM();

#define I2S_WS 26
#define I2S_SD 33
#define I2S_SCK 27

// Use I2S Processor 0
#define I2S_PORT I2S_NUM_0

// Define input buffer length
#define bufferLen 200