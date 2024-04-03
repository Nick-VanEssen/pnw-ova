#pragma once // prevent file from being included more than once

////////////////////////GENERAL SETTINGS/////////////////
#define SERIAL_COM_SPEED 922190
#define LED_PIN 2
#define MAIN_LOOP_DELAY 10
// Uncomment to display free stack in serial terminal
// #define DEBUG_STACK
#ifdef DEBUG_STACK
#define DEBUG_PRINT_INTERVAL 15000
#endif
// #####################################################//
////////////////////////EMAIL SETTINGS///////////////////
// The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

// The sign in credentials
#define AUTHOR_EMAIL "open.vibration.analysis@gmail.com"
#define AUTHOR_PASSWORD "eruu hmqq mpab vxzp"

// Recipient's email
#define RECIPIENT_EMAIL "test@test.com" // Not using anymore
extern char userEmail[];                // Dynamic variable from front end
// #####################################################//
////////////////////////PDM MIC SETTINGS/////////////////
// #define DEBUG_PDM_SERIAL true
#define DEBUG_PDM_LED true
#define PDM_DEBUG_RANGE_LIMIT 1000
#define LED_CHANNEL 0
#define LED_FREQ 5000
#define LEDC_TIMER_12_BIT 12
#define PDM_LED_DELAY 5
// Pins
#define I2S_WS 26
#define I2S_SD 33
#define I2S_SCK 27
// Use I2S Processor 0
#define I2S_PORT I2S_NUM_0
// Define input buffer length
#define PDM_BUFFER_LEN 1024
#define PDM_SAMPLE_RATE 16000
// Task defines
#define PDM_STACK_SIZE 1024 * 18
#define PDM_TASK_PRIORITY 10
#define PDM_TASK_CORE 0
#define PDM_TASK_DELAY 1000
// Debug Task Defines
#define PDM_DEBUG_STACK_SIZE 1000
#define PDM_DEBUG_TASK_PRIORITY 3
#define PDM_DEBUG_TASK_CORE 0
// #####################################################//

////////////////////////ACC SETTINGS/////////////////
#define ACC_BUFFER_LEN 1024
#define ACC_TASK_DELAY 100
#define ACC_SAMPLE_FREQUENCY 3200                          // hz
#define ACC_SAMPLE_DELAY (1 / ACC_SAMPLE_FREQUENCY) * 1000 // 3200hz above equates to .3125 period.
// Task defines
#define ACC_STACK_SIZE 1024 * 18
#define ACC_TASK_PRIORITY 10
#define ACC_TASK_CORE 1
#define ACC_LOOP_DELAY 1000 // how many ms to delay between data collection cycles
// #####################################################//

////////////////////////MON SETTINGS/////////////////
#define MON_BUFFER_LEN 200
#define MON_TASK_DELAY 100
// Task defines
#define MON_STACK_SIZE 5200
#define MON_TASK_PRIORITY 10
#define MON_TASK_CORE 1
                         // #####################################################//

// uncomment to print the raw data to the serial terminal (Consumes significant resources)
#define PRINT_DATA