#include <Arduino.h> 
#include <Preferences.h>
#include "main.h"
#include "get_stored_data.h"
#include "gprs_init_device_registration.h"
#include "accesspoint.h"
#include "push_DTC.h"
#include "Stream_PID_data.h"
#include "OTA_updates.h"

// define all variables stored during device initialization
bool IS_INITIALIZED;
bool IS_ENROLED; 
String firmware_version = "1.0.0"; 

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
String deviceId; 
String storedDevId; 
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

const char* devId;
bool currentEngineStatus = false;
bool previousEngineStatus = false; 

bool ISENROLLED;
String allDtcs;
String DTCChanges = ""; 

long resetAfterMillis = 6*60*1000; // Reset after 6 minutes.
long lastResetWas;
long now; 
void trackClearDTC(); 
void updateEgineStatus(); 


bool vehicleOn = true; 

void setup(){
    Serial.begin(115200);
    Serial.println("BEGINING DEVICE SETUP!!!");
    Serial.println(" "); 
    check_device_Init(); // check if device is already setup || assigned to a specific car
    while(!IS_INITIALIZED) get_stored_data(); //  if not initialised start the registration Portal 
    modemInitialisation(); 
    get_stored_data(); 
    if(IS_INITIALIZED) establishInternetConnection(); // enstablish an internet connection 
    deviceRegistration(); 
    // Toyota = new ToyotaClass();
    // Toyota->diagInit();   
    postEngineStatus(vehicleOn);  
    PerformAnOTA();                                
}


void loop(){
  now = millis();
  updateEgineStatus(); 
  if(vehicleOn){
   if(vehicleOn){
    updateEgineStatus();
    Serial.println("Getting Live Data....");
    long mileage;
    String liveData = ""; 
    Serial.println(liveData);   
    delay(100);
    if(liveData != "Off"){
     pushLiveDataPayloadToServer(liveData);
    } 
   } else{updateEgineStatus();}

   if(vehicleOn){
     updateEgineStatus();
    Serial.println("Reading DTC......");
    String allDtcs = ""; 
    Serial.println(allDtcs);
    delay(100);
    if(allDtcs != "Off"){
      if(allDtcs != DTCChanges){
      pushDTCPayloadToServer(allDtcs); 
      DTCChanges = allDtcs; 
        }else{Serial.println("No changes on DTCs detected");}
    } 
   } else{updateEgineStatus();}
    if(vehicleOn){
    updateEgineStatus();
    trackClearDTC(); 
    } else{updateEgineStatus();}
    if(now > resetAfterMillis){
       lastResetWas = now;
      ESP.restart(); 
    }

  }else{
    updateEgineStatus();
    Serial.println("ECUs OFF - Turn on ignition to atleast position 2 to read ECU");
    Serial.print("Lapsed time is: "); 
    Serial.println(now); 
    // Reset ESP
    if (now > resetAfterMillis){
        lastResetWas = now;
        ESP.restart(); 
    }
  }
    delay(2000);
 }

void updateEgineStatus(){
  // currentEngineStatus = vehicleOn; 
  Serial.print("Current Engine Status: "); 
  Serial.println(currentEngineStatus);
  if(currentEngineStatus != previousEngineStatus){
    // postEngineStatus(vehicleOn); 
    previousEngineStatus =  currentEngineStatus; 
    Serial.print("Current Engine Status: "); 
    Serial.println(currentEngineStatus);
    Serial.print("previous Engine Status: "); 
    Serial.println(previousEngineStatus);
    }else{
     Serial.println("Engine status has not changed Since Last Check"); 
    }
}

void trackClearDTC(){
    getClearDTCStatus(); 
     if(clearDirective){
    Serial.println("Clear directive Initiated, Please Clear...!"); 
    Serial.println();
    Serial.println();
    Serial.println("Clearing DTC......");
    // String clearDTC = Toyota->clearAllDTC(storedDevId);
    // Serial.println(clearDTC);
    delay(100);
    }
    if(clearDTCStatus){
    postClearDTCStatus(); 
    Serial.println();
    Serial.println();
    Serial.println("Reading DTC......");
    // String dtcString2 = Toyota->readAllEcuDtcs(storedDevId);
    // Serial.println(dtcString2);
    delay(100);
    // if(dtcString2 != "Off")
    // pushDTCPayloadToServer(dtcString2); 
    }
} 