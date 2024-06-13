#include <Arduino.h>
#include <WiFi.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>
extern Preferences preferences;

#define WIFI_SSID "ScanWize Toyota"
#define WIFI_PWD "12345678!"


void accessPointInit (bool debug);
void serveHTML(bool debug);