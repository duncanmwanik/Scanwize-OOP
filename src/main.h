#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Preferences.h>
#include "device.h"
#include "modem.h"
#include "network.h"
#include "accesspoint.h"
#include "ble.h"
#include "ota.h"
#include "test.h"

#define IS_DEBUG_MODE true
// custom shortened serial debug
#define logn Serial.print
#define log Serial.println

#define FIRMWARE_VERSION "1.0.0"
#define RESET_AFTER_MILLIS 6 * 60 * 1000 // Reset after 6 minutes.
#define TAMPER_PIN 21
#define R1 1000.0
#define R2 250.0
#define BAT_PIN 34
#define BAT_PIN_EN 22

extern TestVehicleClass vehicle;
extern Device device;
extern Modem modem;
extern Network network;
extern OTA ota;
extern AccessPoint accessPoint;
extern BLE ble;

extern Preferences preferences;

#endif