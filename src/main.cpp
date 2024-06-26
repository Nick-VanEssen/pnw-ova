#define FORMAT_LITTLEFS_IF_FAILED true

// Import required libraries
#include <LittleFS.h>
#include <WiFiManager.h>
#include "pdm_mic.h"
#include "settings.h"
#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>
#include <acc.h>
#include <main.h>
#include <FFT.h>
#include <WiFi.h>
#include <global.h>
#include <monitor.h>

bool ledState = 0;
const int ledPin = 2;

char userEmail[50] = "open.vibration.analysis@gmail.com"; // Default before one is set on frontend
high_resolution_clock::time_point start;
DNSServer dnsServer;
WiFiManager wm;
AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");

// Function to send FFT data
char *sendFFTData()
{
  // No need to calc doc size for v7 because it is dynamic, but we need a size for char array
  JsonDocument doc;

  JsonArray magnitude = doc["magnitude"].to<JsonArray>();
  JsonArray freq = doc["freq"].to<JsonArray>();

  for (int i = 0; i < 1024; i += 2)
  {
    double magValue = accdata.accFFTData[i]; // Average of i and i+1, truncated to 2 decimal places
    magnitude[i / 2] = magValue;
    freq[i / 2] = i;
  }

  doc.shrinkToFit();

  size_t neededSize = measureJson(doc) + 1; // +1 for null terminator
  char *jsonString = new char[neededSize];

  // Serialize the JSON document to the char array
  serializeJson(doc, jsonString, neededSize);

  // Serial.printf("Free Heap: %d \n", ESP.getFreeHeap());
  // Serial.printf("Best Block: %d \n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

  // Return the JSON string. Caller is responsible for deleting it after use.
  return jsonString;
}

void notifyClients(char *data)
{
  websocket.textAll(data);
  delete[] data; // Make sure to free the allocated memory after sending
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "getReadings") == 0)
    {
      char *sensorReadings = sendFFTData();
      notifyClients(sensorReadings);
    }
    if (strchr((char *)data, '@') != nullptr)
    {
      strncpy(userEmail, (char *)data, sizeof(userEmail) - 1); // Copy data into userEmail
      userEmail[sizeof(userEmail) - 1] = '\0';                 // Ensure null-termination
      Serial.print(userEmail);
    }
    if (strcmp((char *)data, "Start") == 0)
    {
      stopstarttoggle.stopStartFlag = 1;
    }
    if (strcmp((char *)data, "Stop") == 0)
    {
      stopstarttoggle.stopStartFlag = 0;
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

  server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/favicon.png", "image/png"); });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/script.js", "text/javascript"); });

  server.begin();

  websocket.onEvent(onEvent);
  server.addHandler(&websocket);

  // pdm.setup();
  acc.setup();
  // mon.setup();
  // startTime();
}

void loop()
{
  vTaskDelay(MAIN_LOOP_DELAY / portTICK_PERIOD_MS);
  // Set as static so it is only initialized once

  wm.process();

  // Send fft data after 1 second
  static unsigned long lastFFTDataSendTime = 0;
  if (millis() - lastFFTDataSendTime > 1000 && stopstarttoggle.stopStartFlag == 1)
  {
    notifyClients(sendFFTData());
    lastFFTDataSendTime = millis();
  }
  websocket.cleanupClients();
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
