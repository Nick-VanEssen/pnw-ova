#pragma once //prevent file from being included more than once
#include "parameters.h"
#define HTTP_SERVER_LOG_TAG "HTTP_Server"

// Replace with your network credentials
const char* ssid = "Kenwood Boys";
const char* password = "pnwbaseball";

const char *accessSsid = "ESP32";
const char *accessPassword = "YourPSKHere";

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

extern userParameters userConfig;

class HTTP_Server {
 private:
 public:
  bool internetConnection;

  void start();
  void stop();
  static void handleLittleFSFile();

  HTTP_Server() { internetConnection = false; }
};
