#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include <otadrive_esp.h>

class Device
{
public:
  bool isFirstTime = true;
  String networkMode = "";
  String hotspotSSID = "";
  String hotspotPASS = "";

  bool isEngineOn = true;
  bool previousEngineStatus = false;

  float latitude = -1.216069;
  float longitude = 36.833663;

  String liveData = "";
  String previousLiveData = "";
  String DTCData = "";
  String previousDTCData = "";

  long currentMillis;

  String deviceId = "";
  String firstName;
  String lastName;
  String username;
  String email;
  String telephone;
  String registrationNumber;
  String whatsappTelephone;
  String vehicleRegistrationNumber;
  String vehicleMake;
  String model;
  String yearOfMake;
  String vehicleTelephone;
  String chasisNumber;
  String imei;
  String fuelType;
  String engineCapacity;
  String engineCode;
  String mileageAtInstallation;
  String batteryBrand;
  String carColor;
  String mileageUnit;
  String gender;
  String batchNumber;
  String serialNumber;
  String productType;
  String transmissionType;
  String tyreBrand;
  String dealerNumber;

  void createCoreTasks();

  void getStoredData();

  bool isInitialized();
  bool isRegistered();

  void readDiagnosticData();
  bool checkEngineStatus();
  void readLiveData();
  void readDTCs();

  bool checkEngineStatus(bool engineStatus);
  bool checkDTCClearDirective();
  void checkBatteryVoltage(float batteryVoltage);

  void sendLiveData();
  void sendDTCData();

  bool checkIfTampered();
  void doReset();
};

#endif