#ifndef TEST_H
#define TEST_H

#include <Arduino.h>

class TestVehicleClass
{
public:
  void diagInit();
  bool ecuOn();
  float batteryVoltageEcuOff(float r1, float r2, int batPin);
  String readLiveData();
  String readAllEcuDtcs();
  String clearAllDTC();
};

#endif