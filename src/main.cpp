#include <Arduino.h>
#include <Preferences.h>
#include "main.h"

Preferences preferences;
TestVehicleClass vehicle;
Device device;
OTA ota("93a0243e-abcd-abcd-abcd-a49c2c7b2c14", FIRMWARE_VERSION);
Modem modem;
Network network;
AccessPoint accessPoint;
BLE ble;

void setup()
{
  if (IS_DEBUG_MODE)
    Serial.begin(115200);

  log("\n---------- Starting device operation ----------\n");

  while (!device.isInitialized())
    accessPoint.start();

  ble.initialize();

  device.getStoredData();

  modem.createInternetConnection();

  while (!device.isRegistered())
    network.registerDevice();

  vehicle.diagInit();

  device.checkEngineStatus(vehicle.ecuOn());

  ota.checkOTA();

  device.createCoreTasks();
}

void loop()
{
}
