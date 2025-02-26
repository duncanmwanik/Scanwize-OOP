#ifndef OTA_H
#define OTA_H

#include <Arduino.h>

class OTA
{
private:
  String OTAKey;
  String firmware_version;

public:
  OTA(String OTAKey_, String firmware_version_);
  void initOTA();
  void checkOTA();
};

#endif