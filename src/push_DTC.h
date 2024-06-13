#include <Arduino.h> 
#include "main.h"

// extern String allDTCs;
extern const char resource_DTC[];

void DTCPayload(String dtcString); 
void pushDTCPayloadToServer(); 
