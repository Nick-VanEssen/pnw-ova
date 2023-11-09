#define FORMAT_LITTLEFS_IF_FAILED true

// Import required libraries
#include <LittleFS.h>
#include <WiFiManager.h>
#include <main.h>
#include <acc.h>

bool ledState = 0;
const int ledPin = 2;
high_resolution_clock::time_point start;

DNSServer dnsServer;

String processor(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (ledState)
    {
      return "ON";
    }
    else
    {
      return "OFF";
    }
  }
  return String();
}

void startTime() {
  start = high_resolution_clock::now();
}

high_resolution_clock::time_point getStartTime() {
  return start;
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  LittleFS.begin();
  accSetup();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // // WiFi Manager setup
  // WiFiManager wm;
  // bool res;
  // res = wm.autoConnect("ESP32", "password"); // ssid and password for access point
  // if (!res)
  // {
  //   Serial.println("Failed to connect"); // print results
  // }
  // Serial.println("Connection Successful!");
  startTime();
}

void loop()
{
  accLoop();
  digitalWrite(ledPin, ledState);
}

