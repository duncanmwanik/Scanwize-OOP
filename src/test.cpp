#include <Arduino.h>
#include "test.h"
#include "main.h"

void TestVehicleClass::diagInit()
{
}

//
//

bool TestVehicleClass::ecuOn()
{
    return true;
}

//
//

float TestVehicleClass::batteryVoltageEcuOff(float r1, float r2, int batPin)
{
    return 11.7;
}

//
//

String TestVehicleClass::clearAllDTC()
{
    delay(2000);
    return "success";
}

//
//

String TestVehicleClass::readLiveData()
{
    String data = "";

    DynamicJsonDocument liveJson(1024);
    liveJson["deviceId"] = device.deviceId;
    liveJson["Distance_traveled_with_malfunction_indicator_lamp_MIL_on"] = 0;
    liveJson["Time_run_with_MIL_on"] = 0;
    liveJson["Vehicle_Speed"] = random(0, 8) * 10;
    liveJson["Run_time_since_engine_start"] = 304;
    liveJson["Distance_traveled_since_codes_cleared"] = 57;
    liveJson["Control_module_voltage"] = 12.89;
    liveJson["Throttle_position"] = random(0, 8) + 18.52941176;
    liveJson["Time_since_trouble_codes_cleared"] = 4590;
    liveJson["Engine_RPM"] = random(0, 8);
    liveJson["Engine_coolant_temperature"] = 24;
    liveJson["MAF_air_flow_rate"] = 6.84;
    liveJson["Air_intake_temperature"] = 23;
    liveJson["latitude"] = device.latitude;
    liveJson["longitude"] = device.longitude;
    ;
    delay(1000);
    data = "";
    serializeJson(liveJson, data);

    return data;
}

//
//

String TestVehicleClass::readAllEcuDtcs()
{
    String data = "";

    DynamicJsonDocument dtcJson(1024);
    dtcJson["deviceId"] = device.deviceId;
    // dtcJson["ECM"] = "P0113|Current,P0102|History,";
    dtcJson["ECM"] = "P011" + String(random(0, 3)) + "|Current,P0102|History,";
    vTaskDelay(10000);
    data = "";
    serializeJson(dtcJson, data);

    return data;
}

//
//
