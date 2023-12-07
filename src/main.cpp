#define FORMAT_LITTLEFS_IF_FAILED true

// Import required libraries
#include <LittleFS.h>
#include <WiFiManager.h>
#include "pdm_mic.h"
#include "email.h"
#include "settings.h"
#include "ESPAsyncWebServer.h"
#include "JSONVar.h"
#include "JSON.h"

DNSServer dnsServer;
WiFiManager wm;
MAILRESULTS mailResults;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
JSONVar readings;

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

// void sendFFTData() {
//     FFTData fftData = getFFTData();

//     JSONVar fftJson;
//     fftJson["magnitude"] = fftData.magnitude;
//     fftJson["frequency"] = fftData.frequency;
    
//     String jsonString = JSON.stringify(fftJson);
//     ws.textAll(jsonString);
// }

// WebSocket event handler
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected from %s\n", 
                  client->id(), client->remoteIP().toString().c_str());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    // Handle data received
  }
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
    Serial.println("Failed to connect");
  }
  else
  {
    Serial.println("Connection Successful!");
  }

LittleFS.begin();
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.begin();

  pdm.setup();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

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

  // Memory debug data
  if (millis() - intervalTimer > DEBUG_PRINT_INTERVAL)
  {
    intervalTimer = millis();
    Serial.printf("Free Heap: %d \n", ESP.getFreeHeap());
    Serial.printf("Best Block: %d \n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    pdm.printMemoryUsage();
  }
}