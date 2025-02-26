#ifndef BLE_H
#define BLE_H

#include <Arduino.h>

#define BLE_NAME "ScanWize Modelo"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class BLE
{
public:
  bool isInitialized;
  bool isConnected;
  String previousData;
  String currentData;

  void initialize();
  void checkBLEData(String currentData);
};

#endif