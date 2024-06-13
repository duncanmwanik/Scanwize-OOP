#include "OTA_updates.h"
#include "gprs_init_device_registration.h"
#include "main.h"

bool OTAUpdates = true; 
String OTAKey = "93a0243e-6f9f-4e61-a775-a49c2c7b2c14"; 

void update_prgs(size_t i, size_t total){
  SerialMon.printf("upgrade %d/%d   %d%%\n", i, total, ((i * 100) / total));
}

void OTAInit(){ 
  Serial.println("Checking for new Verison");
  SPIFFS.begin(true);
  OTADRIVE.setInfo(OTAKey, firmware_version);
  OTADRIVE.onUpdateFirmwareProgress(update_prgs);
  Serial.printf("Download a new firmware from SIM7000,Chip:%s V=%s\n", OTADRIVE.getChipId().c_str(), OTADRIVE.Version.c_str());
}

void PerformAnOTA(){
 if(getOTAUpdate()){
        OTAInit();
        if(OTAUpdates){
        OTAUpdate();
        }
    }
     OTAUpdates = false; 
}
   