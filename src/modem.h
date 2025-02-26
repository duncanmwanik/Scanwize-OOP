#ifndef MODEM_H
#define MODEM_H

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define SerialAT Serial1
#define UART_BAUD 115200

#define MODEM_DTR 25
#define MODEM_TX 27
#define MODEM_RX 26
#define MODEM_PWRKEY 4
#define MODEM_POWER_ON 33
#define LED_GPIO 13
#define LED_ON HIGH
#define LED_OFF LOW
#define MODEM_RST 32

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <WiFi.h>

extern TinyGsm gsmModem;
extern TinyGsmClient gsmClient;
extern WiFiClient wifiClient;

class Modem
{
public:
    bool isConnected = false;

    void initializeModem();

    void createInternetConnection();

    bool isConnectedToNetwork();

     void performOTAUpdate();

    void getGPSLocation();
};

#endif