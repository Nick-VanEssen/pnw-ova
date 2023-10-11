
// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include "main.h"
#include "builtinPage.h"
#define WIFI_CONNECT_TIMEOUT 10

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;


void notifyClients() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
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

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Connect to Wi-Fi

  WiFi.begin(ssid, password);
  int i = 0;
  Serial.printf("Waiting for connection...");
 while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    vTaskDelay(1000 / portTICK_RATE_MS);
    i++;
    if (i > WIFI_CONNECT_TIMEOUT) {
      i = 0;
      WiFi.disconnect();
      WiFi.mode(WIFI_AP);
      break;
    }
    if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Connected!");
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Starting AP mode because no network available...");
    WiFi.softAP(accessSsid, accessPassword);
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", WiFi.localIP());
    Serial.print("AP started. Please verify.");
    Serial.print(" ");
    // Print ESP Local IP Address
   Serial.println(WiFi.softAPIP());
   
  }


  initWebSocket();

  // Route for root / web page
  server.on("/wifiSetup.html", handleLittleFSFile);

  // Start server
  server.begin();

    void HTTP_Server::handleLittleFSFile() {
    String filename = server.uri();
    int dotPosition = filename.lastIndexOf(".");
    String fileType = filename.substring((dotPosition + 1), filename.length());
    if (LittleFS.exists(filename)) {
      File file = LittleFS.open(filename, FILE_READ);
      if (fileType == "gz") {
        fileType = "html";  // no need to change content type as it's done automatically by .streamfile below VV
      }
      server.streamFile(file, "text/" + fileType);
      file.close();
    } else if (!LittleFS.exists("/index.html")) {
      handleIndexFile();
    } else {
      String outputhtml = "<html><body><h1>ERROR 404 <br> FILE NOT FOUND!" + filename + "</h1></body></html>";
      server.send(404, "text/html", outputhtml);
    }
  }
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}