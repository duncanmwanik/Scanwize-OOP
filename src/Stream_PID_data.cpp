#include <Arduino.h> 
#include <ArduinoJson.h>
#include "get_stored_data.h"
#include "Stream_PID_data.h"

// String liveData; 

const char resource_live_data[] = "/proxy/stream/feed";

void liveDataPayload(String liveData){
 DynamicJsonDocument docLive(1024);
 Serial.println(" ");
 deserializeJson(docLive, liveData);
 Serial.println("Printing Live Data JSON... ");
 Serial.println(liveData); 
}