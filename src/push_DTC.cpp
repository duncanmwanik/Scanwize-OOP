#include "main.h"
#include <ArduinoJson.h>
#include "get_stored_data.h"
#include "gprs_init_device_registration.h"
#include "push_DTC.h"

const char resource_DTC[] = "/proxy/stream/pushDTC"; // /proxy/stream/pushDTC

void DTCPayload(String allDTCs){
 DynamicJsonDocument docDTC(1024);
 deserializeJson(docDTC, allDTCs);
 Serial.println("");
 Serial.println("Printing DTC JSON... ");
 Serial.println(allDTCs); 
 delay(1000);
}