#include "main.h"
#include <Wire.h>
#include <Arduino.h> 
#include "get_stored_data.h"
#include "accesspoint.h"

extern String output;
extern String serverResponse; 
extern bool isConnected; 
extern bool  clearDirective; 
extern bool clearDTCStatus; 

extern const char base_url[];
extern const int  port;
extern String DTCs; 
extern float lat; 
extern float lon; 
extern float speed; 
extern float year; 

void setupModem();
// void gprsInit(); 
void gprsRegistrationJSON();

void modemInitialisation(); 
void establishInternetConnection(); 
void deviceRegistration(); 

void pushDTCPayloadToServer(String allDTCs);
void pushLiveDataPayloadToServer(String liveData); 

void postEngineStatus(bool on); 
bool getClearDTCStatus(); 
void postClearDTCStatus(); 
void modemPowerOn(); 
void getGPSLocation(); 
void OTAUpdate(); 
bool getOTAUpdate(); 


#define MODEM_DTR     25
#define MODEM_TX      27
#define MODEM_RX      26
#define MODEM_PWRKEY     4
#define MODEM_POWER_ON       33
#define LED_PIN     13
#define MODEM_RST            32


#define I2C_SDA              21
#define I2C_SCL              22
#define LED_GPIO             13
#define LED_ON               HIGH
#define LED_OFF              LOW

// #define IP5306_ADDR          0x75
// #define IP5306_REG_SYS_CTL0  0x00

