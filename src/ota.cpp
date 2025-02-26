#include <Arduino.h>
#include <SPIFFS.h>
#include <otadrive_esp.h>
#include "main.h"
#include "ota.h"

OTA::OTA(String OTAKey_, String firmware_version_)
{
  OTAKey = OTAKey_;
  firmware_version = firmware_version_;
}

//
//

void update_prgs(size_t i, size_t total)
{
  Serial.printf("upgrade %d/%d   %d%%\n", i, total, ((i * 100) / total));
}

//
//

void OTA::initOTA()
{
  log("Checking for new version");
  SPIFFS.begin(true);
  OTADRIVE.setInfo(OTAKey, firmware_version);
  OTADRIVE.onUpdateFirmwareProgress(update_prgs);
  Serial.printf("Download a new firmware from SIM7000,Chip:%s V=%s\n", OTADRIVE.getChipId().c_str(), OTADRIVE.Version.c_str());
}

//
//

void OTA::checkOTA()
{
  log();

  if (network.getOTAUpdateDirective())
  {
    initOTA();
    modem.performOTAUpdate();
    network.postDisableUpdateDirective();
  }
}

//
//
