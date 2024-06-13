#define TINY_GSM_MODEM_SIM7000 
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#include <Arduino.h> 
#include "main.h"
#include "get_stored_data.h"
#include <ArduinoJson.h>
#include "accesspoint.h"
#include "push_DTC.h"
#include "gprs_init_device_registration.h"
#include "Stream_PID_data.h"
#include "OTA_updates.h"

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <HTTPClient.h>

bool isConnected = false; 
bool clearDirective; 
bool clearDTCStatus; 
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
#define SerialAT Serial1
// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon
// #define LOGGING  // <- Logging is for the HTTP library
#define UART_BAUD   115200
// Your GPRS credentials, if any
const char apn[]      = "iot.safaricom.com"; //safaricom
const char gprsUser[] = ""; // saf
const char gprsPass[] = ""; // data
// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "YourSSID";
const char wifiPass[] = "YourWiFiPass";
// Server details
const char base_url[]   = "api.proxyauto.co.ke";
const char resource[] = "/proxy/vehicles/enroll-gadget";
const char resource_engine_status[] = "/proxy/vehicles/engine-status/"; 
const char resource_clearDTC [] = "/proxy/vehicles/clear-status/"; 
const char resource_clearDTCPOST[] = "/proxy/vehicles/clear-trouble-codes"; 
const char resource_OTAUpdate [] = "/proxy/vehicles/firmware-status/"; 
const int  port       = 80;  //80
float lat,  lon;
String output; 
TinyGsm        modem(SerialAT);
TinyGsmClient client(modem);
HttpClient    http(client, base_url, port);

void modemMethods(); 

void setupModem(){
Serial.println("Powering up modem"); 
#ifdef MODEM_RST
    // Keep reset high
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);
#endif
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
}


void modemPowerOn(){
#ifdef MODEM_RST
    // Keep reset high
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);
#endif

    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    // Turn on the Modem power first
    digitalWrite(MODEM_POWER_ON, HIGH);
    // Pull down PWRKEY for more than 1 second according to manual requirements
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(100);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);
    // Initialize the indicator as an output
    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, 0);
}

bool Close_serve(){
    modem.sendAT(GF("+HTTPTERM"));//close https
    if (modem.waitResponse(5000L) != 1) {
        DBG(GF("+HTTPTERM"));
        Serial.println("wait not done"); 
        return false;
    }
    modem.sendAT(GF("+SAPBR=0,1"));//close GPRS
    if (modem.waitResponse(5000L) != 1) {
        DBG(GF("+SAPBR=0,1"));
        return false;
    }
    return true;
}

void gprsRegistrationJSON(){
DynamicJsonDocument doc(1024); 
doc[String("firstName")] = firstName; //firstName
doc[String("lastName")] = lastName;  // lastName
doc[String("username")] = username;
doc[String("email")] = email;
doc[String("telephone")] = telephone;
doc[String("registrationNumber")] = registrationNumber;
doc[String("vehicleRegistrationNumber")] = vehicleRegistrationNumber;
doc[String("whatsappTelephone")] = whatsappTelephone;
doc[String("vehicleMake")] = vehicleMake;
doc[String("model")] = model;
doc[String("yearOfMake")] = yearOfMake;
doc[String("vehicleTelephone")] = vehicleTelephone;
doc[String("chasisNumber")] = chasisNumber;
doc[String("imei")] = imei;
doc[String("fuelType")] = fuelType;
doc[String("engineCapacity")] =  engineCapacity;
doc[String("engineCode")] = engineCode;
doc[String("mileageAtInstallation")] = mileageAtInstallation;
doc[String("batteryBrand")] = batteryBrand;
doc[String("carColor")] = carColor;
doc[String("mileageUnit")] = mileageUnit;

doc[String("gender")] = gender;
doc[String("batchNumber")] = batchNumber;
doc[String("productType")] = productType;
doc[String("transmission")] = transmissionType;
doc[String("tyreBrand")] = tyreBrand;
doc[String("dealerNumber")] = dealerNumber;
 
 Serial.println(" ");
 serializeJson(doc, output);
 Serial.println("");
 Serial.println("Printing JSON... ");
 Serial.println(output); 
 delay(1000);
}

void modemInitialisation(){
    delay(10);
    setupModem(); 
      // Set GSM module baud rate and UART pins
    SerialAT.begin(UART_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(1000); //3000
    modem.init();
    imei =  modem.getIMEI();
    preferences.begin("ScanMax", false); 
    preferences.putString("imei", imei); 
    preferences.end(); 
}
 
void establishInternetConnection(){
    modemInitialisation(); 
    modem.setNetworkMode(13);
    delay(2000);
    if (!modem.waitForNetwork()){
    ESP.restart();
    }
    if (modem.isNetworkConnected()) { SerialMon.println("Network connected"); }
    // GPRS connection parameters are usually set after network registration
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    ESP.restart();
    }
    SerialMon.println(" success");
    if (modem.isGprsConnected()) { SerialMon.println("GPRS connected"); }
    isConnected = modem.isGprsConnected(); 
}

void deviceRegistration(){
  check_device_enrolled(); 
  // if device is already enrolled stop sending data again. 
  if(!ISENROLLED){
    gprsRegistrationJSON(); 
    // Perform http requests POST || GET
    if (client.connect(base_url,port)) {
    client.print(String("POST ") + resource + " HTTP/1.1\r\n");
    client.print(String("Host: ") + base_url + "\r\n");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(output.length());
    client.println();
    client.println(output);
    // Handling expected response from the server
    String response = http.responseBody();  // client.available() //
    Serial.print("Server Response: ");
    Serial.println(response); 
      //Get response
    SerialMon.println("----------------------");
    SerialMon.println("START Server response");
    SerialMon.println("----------------------");
    // Create a JSON buffer to store the response
    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, response); 
    if(error){
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
    }
    const char* devId = doc["vehicle"]["deviceId"];
    String Id(devId);
    deviceId = Id; 
    addDeviceIdToPref(); 
    Serial.print("Device Id is: ");
    Serial.println(deviceId);
    (deviceId != "") ? ISENROLLED = true: ISENROLLED = false; 
    (ISENROLLED == 1) ? Serial.println("Device ID added Successfully.") : Serial.println("Device ID not found!");
    addIsEnrolledToPref();
    ESP.restart();
    delay(100); 
    }
  }

}
  void modemMethods(){
    Serial.print("battery charge state: "); 
    Serial.println(modem.getBattChargeState()); 
    Serial.print("battery percent: "); 
    Serial.println(modem.getBattPercent());
    Serial.print("Location: "); 
    Serial.println(modem.getGsmLocation()); 
    Serial.print("Location Raw: "); 
    Serial.println(modem.getGsmLocationRaw());   
    Serial.print("Signal Quality:  ");  
    Serial.println(modem.getSignalQuality());    
  }

void pushDTCPayloadToServer(String allDTCs){
Serial.println("Contacting Server Push....");
while(!isConnected) establishInternetConnection();
Serial.print("isConnected: ");
Serial.println(isConnected);
get_stored_data();
DTCPayload(allDTCs); 
       // Perform http requests POST || GET
if (client.connect(base_url,port)){
Serial.println("Client is now Connected "); 
client.print(String("POST ") + resource_DTC + " HTTP/1.1\r\n");
client.print(String("Host: ") + base_url + "\r\n");
client.println("Connection: close");
client.println("Content-Type: application/json");
client.print("Content-Length: ");
client.println(allDTCs.length());
client.println();
client.println(allDTCs);
  //Get response
SerialMon.println("----------------------");
SerialMon.println("START Server response");
SerialMon.println("----------------------");
uint32_t timeout = millis();
while (millis() - timeout < 5000L){
while (client.available()) {
char c = client.read();
SerialMon.print(c);
timeout = millis();
}}
 }else{Serial.println("Client is NOT Connected ");
 }
}


void pushLiveDataPayloadToServer(String liveData){
while (!isConnected) establishInternetConnection();
Serial.print("isConnected: ");
Serial.println(isConnected);
get_stored_data();
liveDataPayload(liveData); 
// Perform http requests POST || GET
if (client.connect(base_url,port)){
Serial.println("Client is now Connected "); 
client.print(String("POST ") + resource_live_data + " HTTP/1.1\r\n");
client.print(String("Host: ") + base_url + "\r\n");
client.println("Connection: close");
client.println("Content-Type: application/json");
client.print("Content-Length: ");
client.println(liveData.length());
client.println();
client.println(liveData);
  //Get response
SerialMon.println("----------------------");
SerialMon.println("START Server response");
SerialMon.println("----------------------");
uint32_t timeout = millis();
while (millis() - timeout < 5000L) {
while (client.available()) {
// String response = http.responseBody(); 
String response = client.readString(); 
if(response != ""){
  Serial.print("Server Response: ");
  Serial.println(response); 
}
char c = client.read();
SerialMon.print("C is: "); 
SerialMon.println(c);
// String response = client.readString(); 
SerialMon.print("response: "); 
SerialMon.println(response); 
SerialMon.print("End of server response ---------"); 
timeout = millis();
     }
   }
  }
}

void postEngineStatus(bool on){
  while (!isConnected) establishInternetConnection();
    if (client.connect(base_url,port)){
  Serial.println("Client is now Connected "); 
  // bool on = "false"; 
  Serial.print("Engine status is: "); 
  Serial.println(on); 
  DynamicJsonDocument statusDoc(1024); 
  statusDoc[String("on")] = on; //firstName
  String output_engine_status; 
  serializeJson(statusDoc, output_engine_status);
  Serial.println("");
  Serial.println("Printing JSON... ");
  Serial.println(output_engine_status); 

  client.print(String("POST ") + resource_engine_status + storedDevId + " HTTP/1.1\r\n");
  client.print(String("Host: ") + base_url + "\r\n");
  client.println("Connection: close");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(output_engine_status.length());
  client.println();
  client.println(output_engine_status);

  //POST response
  SerialMon.println("----------------------");
  SerialMon.println("START Server response");
  SerialMon.println("----------------------");

  uint32_t timeout = millis();
  while (millis() - timeout < 5000L){
  //while (client.connected() && millis() - timeout < 5000L) {
  // Print available data
  while (client.available()) {
  char c = client.read();
  SerialMon.print(c);
  timeout = millis();
  }}
  } else{Serial.println("Client is NOT Connected ");
  }
}

bool getClearDTCStatus(){
 // Perform http requests POST || GET
  while (!isConnected) establishInternetConnection();
   if (client.connect(base_url,port)){
  SerialMon.println("Performing HTTP GET request...");
  get_stored_data();
  client.print(String("GET ") + resource_clearDTC + storedDevId + " HTTP/1.1\r\n");
  client.print(String("Host: ") + base_url + "\r\n");
  client.print("Connection: close\r\n\r\n");
  client.println();
  uint32_t timeout = millis();
  String response; 
  while (millis() - timeout < 5000L) {
  //while (client.connected() && millis() - timeout < 5000L) {
  // Print available data
  while (client.available()) {
  response = client.readStringUntil('\n'); 
  SerialMon.print(response);
  timeout = millis();
  }}
  
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, response); 
  if(error){
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  }
  clearDirective = doc["clearDirective"]; 
  Serial.print("Clear Directive is: "); 
  Serial.println(clearDirective); 
  SerialMon.println();

  http.stop();  // Shutdown
  SerialMon.println(F("Server disconnected"));
  (clearDirective) ? clearDTCStatus = true : clearDTCStatus = false; 
  Serial.print("clear DTC Directive is: "); 
  Serial.println(clearDTCStatus);
}
  return clearDirective; 
}

void postClearDTCStatus(){
  while (!isConnected) establishInternetConnection();
    if (client.connect(base_url,port)){
  Serial.println("Client is now Connected "); 
  get_stored_data(); 
  String deviceId = storedDevId; 
  DynamicJsonDocument statusDoc(1024); 
  statusDoc[String("deviceId")] = deviceId;  
  statusDoc[String("cleared")] = clearDirective; //getClearDTCStatus()
  String clearDirective_outuput; 
  serializeJson(statusDoc, clearDirective_outuput);
  Serial.println("");
  Serial.println("Printing JSON... ");
  Serial.println(clearDirective_outuput); 
  client.print(String("POST ") + resource_clearDTCPOST +  " HTTP/1.1\r\n");
  client.print(String("Host: ") + base_url + "\r\n");
  client.println("Connection: close");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(clearDirective_outuput.length());
  client.println();
  client.println(clearDirective_outuput);

  //POST response
  SerialMon.println("----------------------");
  SerialMon.println("START Server response");
  SerialMon.println("----------------------");

  uint32_t timeout = millis();
  while (millis() - timeout < 5000L) {
  while (client.available()) {
  char c = client.read();
  SerialMon.print(c);
  timeout = millis();
  }}
  } else{Serial.println("Client is NOT Connected ");
  }
}

void getGPSLocation(){
  modem.enableGPS(); 
  float speed    = 0;
  float alt     = 0;
  int   vsat     = 0;
  int   usat     = 0;
  float accuracy = 0;
  int   year     = 0;
  int   month    = 0;
  int   day      = 0;
  int   hour     = 0;
  int   min      = 0;
  int   sec      = 0;

    for (int8_t i = 15; i; i--) {
    SerialMon.println("Requesting current GPS/GNSS/GLONASS location");
    if (modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,
                     &year, &month, &day, &hour, &min, &sec)) {
      SerialMon.println("Latitude: " + String(lat, 8) + "\tLongitude: " + String(lon, 8));
      SerialMon.println("Speed: " + String(speed) + "\tAltitude: " + String(alt));
      SerialMon.println("Visible Satellites: " + String(vsat) + "\tUsed Satellites: " + String(usat));
      SerialMon.println("Accuracy: " + String(accuracy));
      SerialMon.println("Year: " + String(year) + "\tMonth: " + String(month) + "\tDay: " + String(day));
      SerialMon.println("Hour: " + String(hour) + "\tMinute: " + String(min) + "\tSecond: " + String(sec));
      break;
    } 
    else {
      SerialMon.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
      delay(15000L);
    }
  }
  SerialMon.println("Retrieving GPS/GNSS/GLONASS location again as a string");
  String gps_raw = modem.getGPSraw();
  SerialMon.println("GPS/GNSS Based Location String: " + gps_raw);
  SerialMon.println("Disabling GPS");
}

bool getOTAUpdate(){
  bool OTAUpdate;
 // Perform http requests POST || GET
  while (!isConnected) establishInternetConnection();
   if (client.connect(base_url,port)){
  SerialMon.println("Performing HTTP GET request...");
  get_stored_data();
  client.print(String("GET ") + resource_OTAUpdate + storedDevId + " HTTP/1.1\r\n");
  client.print(String("Host: ") + base_url + "\r\n");
  client.print("Connection: close\r\n\r\n");
  client.println();
  uint32_t timeout = millis();
  String response; 
  while (millis() - timeout < 5000L) {
  //while (client.connected() && millis() - timeout < 5000L) {
  // Print available data
  while (client.available()) {
  response = client.readStringUntil('\n'); 
  SerialMon.print(response);
  timeout = millis();
  }}
  
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, response); 
  if(error){
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  }
  OTAUpdate = doc["updateDirective"]; 
  Serial.println("");
  Serial.print("updateDirective is: "); 
  Serial.println(OTAUpdate); 
  SerialMon.println();

  http.stop();  // Shutdown
  SerialMon.println(F("Server disconnected"));
}
  return OTAUpdate; 
}

void OTAUpdate(){
TinyGsmClient gsm_otadrive_client(modem, 1);
if (OTADRIVE.timeTick(10)){
    Serial.println("Lets update the firmware");
    if(isConnected){
      Serial.print("isGprsConnected: ");
      Serial.println(isConnected);
      OTADRIVE.updateFirmware(gsm_otadrive_client);
    } else{
      Serial.println("Modem is not ready");
    }
  }
  // updateAll(); 
  // Update the Version here; 
}