#include <Arduino.h>
#include "device.h"
#include "main.h"

//
//

void Device::readDiagnosticData()
{
  if (checkEngineStatus(vehicle.ecuOn()))
  {
    // ---------- Read live data 10 times
    // for (int i = 0; i < 10; i++)
    // {
    liveData = vehicle.readLiveData();

    sendLiveData();
    sendDTCData();
    // }

    if (network.getClearDTCStatus())
    {
      logn("Clearing DTCs...");
      String clearDTC = vehicle.clearAllDTC();
      log(clearDTC);

      network.postDisableClearDTCStatus();
    }
  }
  else
  {
    checkBatteryVoltage(vehicle.batteryVoltageEcuOff(R1, R2, BAT_PIN));
  }

  doReset();
}

//
//

void Device::readDTCs()
{
  if (isEngineOn)
  {
    DTCData = vehicle.readAllEcuDtcs();
  }
}

//
//

bool Device::checkEngineStatus(bool engineStatus)
{
  isEngineOn = engineStatus;

  if ((isEngineOn != previousEngineStatus) || isFirstTime)
  {
    previousEngineStatus = isEngineOn;
    network.postEngineStatus(isEngineOn);
    isFirstTime = false;
  }

  if (!isEngineOn)
    log("-Engine is off...");

  return isEngineOn;
}

//
//

void Device::checkBatteryVoltage(float batteryVoltage)
{
  log("Battery voltage is: " + String(batteryVoltage));

  if (batteryVoltage < 11)
    network.postBatteryVoltage(batteryVoltage);
}

//
//

void Device::sendLiveData()
{
  if (liveData != "Off")
  {
    if (liveData != previousLiveData)
    {
      previousLiveData = liveData;

      logn("\n------ Sending new live data: ");
      log(liveData);
      network.postLiveData(liveData);
      log();
    }
    else
    {
      log("No changes in Live Data...");
    }
  }
}

//
//

void Device::sendDTCData()
{
  if (DTCData != "Off")
  {
    if (DTCData != previousDTCData)
    {
      previousDTCData = DTCData;

      logn("\n------ Sending new DTCs data: ");
      log(DTCData);
      network.postDTCData(DTCData);
      log();
    }
    else
    {
      log("No changes in DTCs Data...");
    }
  }
}

//
//

void Device::doReset()
{
  currentMillis = millis();
  // Reset ESP
  if (currentMillis > RESET_AFTER_MILLIS)
  {
    log("\n---------- Restarting device after 6 minutes ----------\n");
    ESP.restart();
  }
}

//
//

bool Device::checkIfTampered()
{
  pinMode(TAMPER_PIN, INPUT);

  bool isTampered = digitalRead(TAMPER_PIN);

  if (isTampered)
  {
    log("Tampered! Restarting ..");
    ESP.restart();
  }

  return isTampered;
}

//
//

bool Device::isInitialized()
{
  preferences.begin("ScanMax", false);
  bool isInitialized = preferences.getBool("isInitialized", false);
  preferences.end();

  if (!isInitialized)
    log("Device is not initialized...");

  return isInitialized;
}

//
//

bool Device::isRegistered()
{
  delay(100);

  preferences.begin("ScanMax", false);
  bool isRegistered = preferences.getBool("isRegistered", false);
  preferences.end();

  return isRegistered;
}

//
//

void Device::getStoredData()
{
  preferences.begin("ScanMax", false);
  firstName = preferences.getString("firstName", "");
  lastName = preferences.getString("lastName", "");
  email = preferences.getString("email", "");
  telephone = preferences.getString("telephone", "");
  vehicleRegistrationNumber = preferences.getString("vehicleReg", "");
  whatsappTelephone = preferences.getString("WhatsappNo", "");
  vehicleMake = preferences.getString("vehicleMake", "");
  model = preferences.getString("model", "");
  yearOfMake = preferences.getString("yearOfMake", "");
  chasisNumber = preferences.getString("chasisNumber", "");
  imei = preferences.getString("imei", "");
  deviceId = preferences.getString("deviceId", "99c66df1-2a9c-457b-94e9-3dc276652fb6");
  // deviceId = "99c66df1-2a9c-457b-94e9-3dc276652fb6";
  fuelType = preferences.getString("fuelType", "");
  engineCapacity = preferences.getString("engineCapacity", "");
  engineCode = preferences.getString("engineCode", "");
  mileageAtInstallation = preferences.getString("mileage", "");
  batteryBrand = preferences.getString("batteryBrand", "");
  carColor = preferences.getString("carColor", "");
  mileageUnit = preferences.getString("mileageUnit", "");
  gender = preferences.getString("gender", "");
  batchNumber = preferences.getString("batchNumber", "");
  productType = preferences.getString("productType", "");
  transmissionType = preferences.getString("transmission", "");
  tyreBrand = preferences.getString("tyreBrand", "");
  dealerNumber = preferences.getString("dealerNumber", "");
  serialNumber = preferences.getString("serialNumber", "");
  networkMode = preferences.getString("networkMode", "gsm");
  hotspotSSID = preferences.getString("hotspotSSID", "---");
  hotspotPASS = preferences.getString("hotspotPASS", "---");
  preferences.end();

  // if (!SPIFFS.begin(true))
  // {
  //   log("An Error has occurred while mounting SPIFFS...");
  //   return;
  // }
  // File file = SPIFFS.open("/id.txt");
  // if (!file)
  // {
  //   log("There was an error opening the file for writing...");
  //   return;
  // }
  // String id = file.readString();
  // log("ID: " + id);
  // file.close();
}

//
//

void core0Task(void *)
{
  for (;;)
  {
    device.readDTCs();

    vTaskDelay(10);
  }
}

void core1Task(void *)
{
  for (;;)
  {
    device.readDiagnosticData();

    vTaskDelay(10);
  }
}

void Device::createCoreTasks()
{
  xTaskCreatePinnedToCore(
      core0Task,   // task function.
      "core0Task", // name of task.
      10000,       // stack size of task
      NULL,        // parameter of the task
      1,           // priority of the task
      NULL,        // task handle to keep track of created task
      0);

  xTaskCreatePinnedToCore(
      core1Task,   // task function.
      "core1Task", // name of task.
      10000,       // stack size of task
      NULL,        // parameter of the task
      1,           // priority of the task
      NULL,        // task handle to keep track of created task
      1);

  log("Created core tasks...\n");
}

//
//
