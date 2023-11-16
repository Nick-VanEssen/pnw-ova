#define FORMAT_LITTLEFS_IF_FAILED true

// Import required libraries
#include <LittleFS.h>
#include <WiFiManager.h>
#include <acc.h>

bool ledState = 0;
const int ledPin = 2;

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

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  LittleFS.begin();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // WiFi Manager setup
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("ESP32", "password"); // ssid and password for access point
  if (!res)
  {
    Serial.println("Failed to connect"); // print results
  }
  Serial.println("Connection Successful!");
  setupAcc();
}

void loop()
{
  loopAcc();
  digitalWrite(ledPin, ledState);
}

