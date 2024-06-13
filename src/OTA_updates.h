#include <Arduino.h>
#include <otadrive_esp.h>
#define SerialMon Serial

void OTAInit(); 
void update_prgs(size_t i, size_t total); 
void PerformAnOTA(); 