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
#include <acc.h>
#include <main.h>
#include <FFT.h>
#include <WiFi.h>
#include <global.h>
#include <monitor.h>

bool ledState = 0;
const int ledPin = 2;

high_resolution_clock::time_point start;
DNSServer dnsServer;
WiFiManager wm;
MAILRESULTS mailResults;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
String sensorReadings;
String jsonString;

JSONVar fftJson;
JSONVar magArray;

// WebSocket function
String sendFFTData()
{
  fftJson["magnitude"] = accdata.accFFTData[5];
  fftJson["freq"] = 0; // TODO
  Serial.print(ESP.getHeapSize());
  Serial.printf("Best Block: %d \n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

  jsonString = JSON.stringify(fftJson);
  return jsonString;
}

void notifyClients(String sensorReadings)
{
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    String message = (char *)data;
    // Check if the message is "getReadings"
    if (strcmp((char *)data, "getReadings") == 0)
    {
      // if it is, send current sensor readings
      String sensorReadings = sendFFTData();
      Serial.print(sensorReadings);
      notifyClients(sensorReadings);
    }
  }
}
// WebSocket event handler
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void startTime()
{
  start = high_resolution_clock::now();
}

high_resolution_clock::time_point getStartTime()
{
  return start;
}

// Find IP of board, set it as string to be used in index. Most likely not used anymore.
// String processor(const String &var)
// {

//   if (var == "IPaddress")
//   {
//     return WiFi.localIP().toString();
//   }
//   return String();
// }

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(SERIAL_COM_SPEED);
  LittleFS.begin();

  // configure LED PWM functionalitites
  ledcSetup(LED_CHANNEL, 5000, 8);

  // // attach the channel to the GPIO to be controlled
  // // to drive the LED, just set a value from 0-255
  // // using ledcWrite(LED_CHANNEL, Brightness)
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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });
  server.serveStatic("/", LittleFS, "/");

  server.begin();

  ws.onEvent(onEvent);
  server.addHandler(&ws);

  pdm.setup();
  acc.setup();
  mon.setup();
  startTime();
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

  // Send fft data after 1 second
  static unsigned long lastFFTDataSendTime = 0;
  if (millis() - lastFFTDataSendTime > 1000)
  {
    sensorReadings = sendFFTData();
    lastFFTDataSendTime = millis();
    notifyClients(sensorReadings);
  }
  ws.cleanupClients();
// Memory debug data
#ifdef DEBUG_STACK
  if (millis() - intervalTimer > DEBUG_PRINT_INTERVAL)
  {
    intervalTimer = millis();
    Serial.printf("Free Heap: %d \n", ESP.getFreeHeap());
    Serial.printf("Best Block: %d \n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    pdm.printMemoryUsage();
    acc.printMemoryUsage();
  }
#endif
}
