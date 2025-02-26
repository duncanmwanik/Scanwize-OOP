#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <SSLClient.h>
#include <ArduinoJson.h>
#include "main.h"

SSLClient secureClientGSM(&gsmClient);
SSLClient secureClientWiFi(&wifiClient);
HttpClient https(secureClientGSM, BASE_URL, PORT);

//
//

void Network::setup()
{
  if (device.networkMode == "gsm")
  {
    https = HttpClient(secureClientGSM, BASE_URL, PORT);
    secureClientGSM.setCACert(rootCACertificate);
  }
  else
  {
    https = HttpClient(secureClientWiFi, BASE_URL, PORT);
    secureClientWiFi.setCACert(rootCACertificate);
  }
}

//
//

StaticJsonDocument<2048> Network::postHttpRequest(String resource, String data)
{
  StaticJsonDocument<2048> responseJson;

  if (modem.isConnectedToNetwork())
  {
    https.connectionKeepAlive();

    log("POST Request-> " + resource);
    https.post(resource, "Content-Type: application/json", data);
    int status_code = https.responseStatusCode();
    String response = https.responseBody();

    Serial.print("Status code: ");
    Serial.println(status_code);
    Serial.print("Response: ");
    Serial.println(response);

    https.stop();
  }

  return responseJson;
}

//
//

StaticJsonDocument<2048> Network::getHttpRequest(String resource)
{
  StaticJsonDocument<2048> responseJson;

  if (modem.isConnectedToNetwork())
  {
    https.connectionKeepAlive();

    log("GET Request-> " + resource);
    https.get(resource);
    int status_code = https.responseStatusCode();
    String response = https.responseBody();

    Serial.print("Status code: ");
    Serial.println(status_code);
    Serial.print("Response: ");
    Serial.println(response);

    // Creating a JSON buffer to store the response
    DeserializationError error = deserializeJson(responseJson, response);

    https.stop();
  }

  return responseJson;
}

//
//

void Network::registerDevice()
{
  log("\nRegistering device...");

  String registrationJson = getRegistrationJson();

  logn("\nRegistering JSON: ");
  log(registrationJson);

  StaticJsonDocument<2048> responseJson = postHttpRequest(resourceEnrollDevice, registrationJson);

  const char *devId = responseJson["vehicle"]["deviceId"];
  String id(devId);
  if (id != "")
  {
    preferences.begin("ScanMax", false);
    // save device id
    preferences.putString("deviceId", id);
    device.deviceId = id;
    // update isRegistered
    preferences.putBool("isRegistered", true);
    preferences.end();
    //

    logn("Registration Success! Device Id is: ");
    log(id);
  }
  else
  {
    log("Failed to get Device Id! Restarting...\n");
  }
}

//
//

void Network::postLiveData(String liveData)
{
  postHttpRequest(resourceLiveData, liveData);
}

//
//

void Network::postDTCData(String allDTCs)
{
  postHttpRequest(resourceDTC, allDTCs);
}

//
//

void Network::postBatteryVoltage(float batteryVoltage)
{
  DynamicJsonDocument batteryJson(1024);
  batteryJson[String("Control_module_voltage")] = batteryVoltage;
  batteryJson[String("deviceId")] = device.deviceId;
  String batteryStatus;
  serializeJson(batteryJson, batteryStatus);

  log("Posting battery voltage: " + batteryStatus);
  postHttpRequest(resourceBatteryVoltage, batteryStatus);
}

//
//

void Network::postEngineStatus(bool on)
{
  DynamicJsonDocument statusJson(1024);
  statusJson[String("on")] = on;
  String engineStatus;
  serializeJson(statusJson, engineStatus);
  logn("\nPosting Engine status: ");
  log(engineStatus);
  postHttpRequest(resourceEngineStatus + device.deviceId, engineStatus);
  log();
}

//
//

bool Network::getClearDTCStatus()
{
  log();

  StaticJsonDocument<2048> responseJson = getHttpRequest(resourceClearDTC + device.deviceId);

  bool status = responseJson["clearDirective"];
  log("Clear DTC status is: " + String(status) + "\n");

  return status;
}

//
//

void Network::postDisableClearDTCStatus()
{
  log("Posting clear DTC status...");
  DynamicJsonDocument statusJson(1024);
  statusJson[String("deviceId")] = device.deviceId;
  statusJson[String("cleared")] = true;
  String clearDTCstats;
  ;
  serializeJson(statusJson, clearDTCstats);

  postHttpRequest(resourceClearDTCPOST, clearDTCstats);
}

//
//

void Network::postDisableUpdateDirective()
{
  DynamicJsonDocument json(1024);
  json["deviceId"] = device.deviceId;
  json["updateDirective"] = false;
  String update;
  serializeJson(json, update);

  postHttpRequest(resourceDisableUpdateOTA, update);
}

//
//

bool Network::getOTAUpdateDirective()
{
  bool otaUpdateDirective = false;

  StaticJsonDocument<2048> responseJson = getHttpRequest(resourceOTAUpdate + device.deviceId);

  otaUpdateDirective = responseJson["updateDirective"];
  logn("OTA updateDirective is: ");
  log(otaUpdateDirective);
  log();

  return otaUpdateDirective;
}

//
//

String Network::getRegistrationJson()
{
  String json;

  DynamicJsonDocument doc(1024);
  doc[String("firstName")] = device.firstName;
  doc[String("lastName")] = device.lastName;
  doc[String("email")] = device.email;
  doc[String("telephone")] = device.telephone;
  doc[String("registrationNumber")] = device.registrationNumber;
  doc[String("vehicleRegistrationNumber")] = device.vehicleRegistrationNumber;
  doc[String("whatsappTelephone")] = device.whatsappTelephone;
  doc[String("vehicleMake")] = device.vehicleMake;
  doc[String("model")] = device.model;
  doc[String("yearOfMake")] = device.yearOfMake;
  doc[String("vehicleTelephone")] = device.vehicleTelephone;
  doc[String("chasisNumber")] = device.chasisNumber;
  doc[String("imei")] = device.imei;
  doc[String("fuelType")] = device.fuelType;
  doc[String("engineCapacity")] = device.engineCapacity;
  doc[String("engineCode")] = device.engineCode;
  doc[String("mileageAtInstallation")] = device.mileageAtInstallation;
  doc[String("batteryBrand")] = device.batteryBrand;
  doc[String("carColor")] = device.carColor;
  doc[String("mileageUnit")] = device.mileageUnit;
  doc[String("gender")] = device.gender;
  doc[String("batchNumber")] = device.batchNumber;
  doc[String("productType")] = device.productType;
  doc[String("transmissionType")] = device.transmissionType;
  doc[String("tyreBrand")] = device.tyreBrand;
  doc[String("dealerNumber")] = device.dealerNumber;
  doc[String("serialNumber")] = device.serialNumber;

  serializeJson(doc, json);

  return json;
}

//
//