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

  // Uncomment the next two lines to see available heap and best block through each serialization.
  // Serial.printf("Free Heap: %d \n", ESP.getFreeHeap());
  // Serial.printf("Best Block: %d \n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

  // Return the JSON string. Caller is responsible for deleting it after use.
  return jsonString;
}

char *sendSensitivityData()
{
  JsonDocument doc;

  JsonArray sensitivity = doc["sensitivity"].to<JsonArray>();
  if (sensitivityValue.sensValue <= 1)
  {
    sensitivity[0] = 10;
  }
  if (sensitivityValue.sensValue > 1 && sensitivityValue.sensValue < 5)
  {
    sensitivity[0] = 10 - sensitivityValue.sensValue;
  }
  else if (sensitivityValue.sensValue == 5)
  {
    sensitivity[0] = sensitivityValue.sensValue;
  }
  else if (sensitivityValue.sensValue < 10 && sensitivityValue.sensValue > 5)
  {
    sensitivity[0] = 10 - sensitivityValue.sensValue;
  }
  else if (sensitivityValue.sensValue >= 10)
  {
    sensitivity[0] = 1;
  }

  doc.shrinkToFit();

  size_t neededSize = measureJson(doc) + 1;
  char *jsonString = new char[neededSize];

  serializeJson(doc, jsonString, neededSize);

  return jsonString;
}

char *sendStopStartData()
{
  JsonDocument doc;

  JsonArray stopStartData = doc["stopstartdata"].to<JsonArray>();
  stopStartData[0] = stopstarttoggle.stopStartFlag;

  doc.shrinkToFit();

  size_t neededSize = measureJson(doc) + 1;
  char *jsonString = new char[neededSize];

  serializeJson(doc, jsonString, neededSize);

  return jsonString;
}

void notifyClients(const char *data, bool dynamic = false)
{
  websocket.textAll(data);
  if (dynamic)
  {
    delete[] data;
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0; // Null-terminate the data to handle it as a string.

    // Convert uint8_t* to char* for string manipulation
    char *charData = (char *)data;

    // Handle string commands
    if (strcmp(charData, "getReadings") == 0)
    {
      char *sensorReadings = sendFFTData();
      notifyClients(sensorReadings, true);
    }
    else if (strcmp(charData, "getSensitivity") == 0)
    {
      char *sens = sendSensitivityData();
      notifyClients(sens, true);
    }
    else if (strcmp(charData, "getStopStart") == 0)
    {
      char *stpstrt = sendStopStartData();
      notifyClients(stpstrt, true);
    }
    else if (strchr(charData, '@') != nullptr)
    {
      strncpy(userEmail, charData, sizeof(userEmail) - 1); // Copy data into userEmail
      userEmail[sizeof(userEmail) - 1] = '\0';             // Ensure null-termination
      Serial.print(userEmail);
    }
    else if (strcmp(charData, "Start") == 0)
    {
      stopstarttoggle.stopStartFlag = 1;
    }
    else if (strcmp(charData, "Stop") == 0)
    {
      stopstarttoggle.stopStartFlag = 0;
    }
    else if (strcmp(charData, "reAverage") == 0)
    {
      reaveragedata.reAverageFlag = 1;
    }

    else
    {
      // If a number gets sent through the websocket, handle it via Json
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, charData);
      if (error)
      {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      if (doc.containsKey("sensitivity"))
      {
        double sensitivity = doc["sensitivity"];
        if (sensitivity <= 1)
        {
          sensitivityValue.sensValue = 10;
        }
        if (sensitivity > 1 && sensitivity < 5)
        {
          sensitivityValue.sensValue = 10 - sensitivity;
        }
        else if (sensitivity == 5)
        {
          sensitivityValue.sensValue = sensitivity;
        }
        else if (sensitivity < 10 && sensitivity > 5)
        {
          sensitivityValue.sensValue = 10 - sensitivity;
        }
        else if (sensitivity >= 10)
        {
          sensitivityValue.sensValue = 1;
        }
      }
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

  acc.setup();
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
    notifyClients(sendFFTData(), true);
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
