// include "modem.h" before <TinyGsmClient.h> to avoid some type errors
#include <Arduino.h>
#include "main.h"

TinyGsm gsmModem(SerialAT);
TinyGsmClient gsmClient(gsmModem);
WiFiClient wifiClient;

void Modem::initializeModem()
{
    delay(10);
    //
    // setup
    // Keep reset high
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);
    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    digitalWrite(MODEM_POWER_ON, HIGH);
    // Pull down PWRKEY for more than 1 second according to manual requirements
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(100);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);
    // Initialize the indicator as an output
    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LED_OFF);
    //
    //
    // Set GSM module baud rate and UART pins
    SerialAT.begin(UART_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(1000); // 3000
    gsmModem.init();
    device.imei = gsmModem.getIMEI();
    preferences.begin("ScanMax", false);
    preferences.putString("imei", device.imei);
    preferences.end();
}

//
//

void Modem::createInternetConnection()
{
    log("network... " + device.networkMode);

    if (device.networkMode == "gsm")
    {
        log("Connecting to GSM network...");

        initializeModem();

        gsmModem.setNetworkMode(13);
        delay(2000);
        if (!gsmModem.waitForNetwork())
        {
            log("GSM reset---X");
            ESP.restart();
        }
        if (gsmModem.isNetworkConnected())
        {
            log("GSM connected.");
        }
        log("Connecting to GPRS...");
        // GPRS connection parameters are usually set after network registration
        if (!gsmModem.gprsConnect(APN, APN_USER, APN_PASS))
        {
            log("GPRS reset---X");
            ESP.restart();
        }
        if (gsmModem.isGprsConnected())
        {
            log("GPRS connected.");
        }

        isConnected = gsmModem.isGprsConnected();
    }
    else
    {
        log("Connecting to WiFi network: " + device.hotspotSSID + " : " + device.hotspotPASS);

        while (WiFi.status() != WL_CONNECTED)
        {
            WiFi.begin(device.hotspotSSID, device.hotspotPASS);
            logn(".");
            delay(1000);
        }
        log("Connected to WiFi network...");
        isConnected = WiFi.isConnected();
    }

    delay(500);

    network.setup();
}

//
//

bool Modem::isConnectedToNetwork()
{
    if (device.networkMode == "gsm")
    {
        isConnected = gsmClient.connect(BASE_URL, PORT);
    }
    else
    {
        isConnected = wifiClient.connect(BASE_URL, 80);
    }

    if (!isConnected)
    {
        log("Lost Connection! Connecting to network...");

        while (!isConnected)
            createInternetConnection();
    }

    return isConnected;
}

//
//

void Modem::performOTAUpdate()
{
    TinyGsmClient gsm_otadrive_client(gsmModem, 1);

    if (OTADRIVE.timeTick(10))
    {
        log("Updating firmware via OTA");
        if (isConnected)
        {
            logn("isGprsConnected: ");
            log(isConnected);
            OTADRIVE.updateFirmware(gsm_otadrive_client);
        }
        else
        {
            log("Modem is not ready");
        }
    }
    // Update the Version here;
}

//
//

void Modem::getGPSLocation()
{
    gsmModem.enableGPS();

    for (int i = 0; i < 15; i++)
    {
        if (gsmModem.getGPS(&device.latitude, &device.longitude))
        {
            log("\nLatitude: " + String(device.latitude, 8) + "\tLongitude: " + String(device.longitude, 8));
            break;
        }
        else
        {
            log("Couldn't get GPS/GNSS/GLONASS location, retrying in 5s.");
            delay(5000L);
        }
    }

    // test delay
    delay(3000);
}

//
//