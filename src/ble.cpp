#include <Arduino.h>
#include <NimBLEDevice.h>
#include "main.h"
#include "ble.h"

static NimBLEServer *pServer;
NimBLECharacteristic *bleCharacteristic;

//
// ---------- ---------- CALLBACKS
//

// Handler class for BLE connection and disconnection
class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer)
    {
        ble.isConnected = true;
        log("App connected via BLE...");
        NimBLEDevice::startAdvertising();
    };
    void onDisconnect(NimBLEServer *pServer)
    {
        ble.isConnected = false;
        log("App disconnected from BLE...");
        NimBLEDevice::startAdvertising();
    };
};

// Handler class for characteristic read or write
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
    void onWrite(NimBLECharacteristic *pCharacteristic)
    {
        ble.checkBLEData(String(pCharacteristic->getValue().c_str()));
    };
};

//
// ---------- ---------- CALLBACKS
//

void BLE::initialize()
{
    log("Setting up BLE service...");

    // sets device name
    NimBLEDevice::init(BLE_NAME);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    NimBLEService *pWiFiService = pServer->createService(SERVICE_UUID);
    NimBLECharacteristic *pWifiCharacteristic = pWiFiService->createCharacteristic(
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE

    );

    pWifiCharacteristic->setValue("CHARACTERISTIC_UUID");
    pWifiCharacteristic->setCallbacks(new CharacteristicCallbacks());

    // Start the services when finished creating all Characteristics
    pWiFiService->start();

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    // Add the services to the advertisment data
    pAdvertising->addServiceUUID(pWiFiService->getUUID());
    // false will extend battery life at the expense of less data sent
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    isInitialized = true;
    log("BLE ready...");
}

//
//

void BLE::checkBLEData(String currentData)
{
    if (currentData != previousData)
    {
        log();
        log("BLE command is: " + currentData);
        if (currentData.startsWith("wifi"))
        {
            // ------------------------------------------ Phase 1

            int comma1 = currentData.indexOf(",");
            int comma2 = currentData.lastIndexOf(",");
            String wifiSSID = currentData.substring(comma1 + 1, comma2);
            String wifiPASS = currentData.substring(comma2 + 1);

            log(wifiSSID + " : " + wifiPASS);

            preferences.begin("ScanMax", false);
            preferences.putString("networkMode", "wifi");
            preferences.putString("hotspotSSID", wifiSSID);
            preferences.putString("hotspotPASS", wifiPASS);
            preferences.end();
            log("Network mode changed to WiFi. Restarting device...\n");
            ESP.restart();
        }
        if (currentData.startsWith("gsm"))
        {
            preferences.begin("ScanMax", false);
            preferences.putString("networkMode", "gsm");
            preferences.end();
            log("Network mode changed to GSM. Restarting device...\n");
            ESP.restart();
        }
    }
}

//
//
