#define FORMAT_LITTLEFS_IF_FAILED true

// Import required libraries
#include <LittleFS.h>
#include <WiFiManager.h>
#include "pdm_mic.h"
#include "email.h"
#include "settings.h"
#include "aWOT.h"

DNSServer dnsServer;
WiFiManager wm;
MAILRESULTS mailResults;

WiFiServer server(80);
Application app;
void index(Request &req, Response &res)
{
  res.print("Hello World!");
}

String processor(const String &var)
{
  /* Serial.println(var);
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
   }*/
  return String();
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(SERIAL_COM_SPEED);
  LittleFS.begin();

  // configure LED PWM functionalitites
  ledcSetup(LED_CHANNEL, 5000, 8);

  // attach the channel to the GPIO to be controlled
  // to drive the LED, just set a value from 0-255
  // using ledcWrite(LED_CHANNEL, Brightness)
  ledcAttachPin(LED_PIN, LED_CHANNEL);

  // WiFi Manager
  bool res;
  wm.setConfigPortalBlocking(false);                  // don't wait for user wifi setup
  res = wm.autoConnect("OVA WiFi Setup", "password"); // ssid and password for access point
  if (!res)
  {
    Serial.println("Failed to connect"); // print results
  }
  Serial.println("Connection Successful!");
  app.get("/", &index);
  server.begin();

  pdm.setup();
}

void emailNotification();

void loop()
{
  vTaskDelay(MAIN_LOOP_DELAY / portTICK_PERIOD_MS);
  // Set as static so it is only initialized once
  static bool _mailSent = false;
  static unsigned long intervalTimer = millis();

  if (!_mailSent)
  {
    // commented out so it doesn't send an email every time
    // mailResults.send();
    _mailSent = true;
  }
  wm.process();
  WiFiClient client = server.available();
  if (client.connected())
  {
    app.process(&client);
  }
  // Memory debug data
  if (millis() - intervalTimer > DEBUG_PRINT_INTERVAL)
  {
    intervalTimer = millis();
    Serial.printf("Free Heap: %d \n", ESP.getFreeHeap());
    Serial.printf("Best Block: %d \n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    pdm.printMemoryUsage();
  }
}