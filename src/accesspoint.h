#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#define WIFI_SSID "ScanWize Toyota"
#define WIFI_PWD "12345678!"

class AccessPoint
{
public:
    void start();
    void serveHTML();
};

#endif