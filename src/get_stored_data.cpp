    #include <Arduino.h> 
    #include "main.h" 
    #include "accesspoint.h"
    #include <Preferences.h>

    bool startAccessPoint = true; 

    bool check_device_Init(){
    preferences.begin("ScanMax", false);
    IS_INITIALIZED = preferences.getBool("IS_INITIALIZED",false);
    preferences.end();
    return IS_INITIALIZED;
    }

    void addDeviceIdToPref(){
    preferences.begin("ScanMax", false); 
    preferences.putString("deviceID", deviceId); 
    preferences.end(); 
    }

    void addIsEnrolledToPref(){
    preferences.begin("ScanMax", false); 
    preferences.putBool("ISENROLLED", ISENROLLED); 
    preferences.end(); 
    }

    bool check_device_enrolled(){
    preferences.begin("ScanMax", false);
    ISENROLLED = preferences.getBool("ISENROLLED",false);
    preferences.end();
    return ISENROLLED;
    }

    void get_stored_data(){
    check_device_Init(); 
    if(IS_INITIALIZED){
    preferences.begin("ScanMax", false); 
    firstName =  preferences.getString("firstName",""); 
    lastName =   preferences.getString("lastName", ""); 
    email = preferences.getString("email", ""); 
    username = preferences.getString("username", ""); 
    telephone= preferences.getString("telephone", "");
    vehicleRegistrationNumber = preferences.getString("vehicleReg", ""); 
    whatsappTelephone = preferences.getString("WhatsappNo", ""); 
    vehicleMake = preferences.getString("vehicleMake", ""); 
    model = preferences.getString("model", ""); 
    yearOfMake = preferences.getString("yearOfMake", ""); 
    chasisNumber = preferences.getString("chasisNumber", ""); 
    imei = preferences.getString("imei", ""); 
    storedDevId = preferences.getString("deviceID", "");
    fuelType = preferences.getString("fuelType", ""); 
    engineCapacity = preferences.getString("engineCapacity", ""); 
    engineCode = preferences.getString("engineCode", ""); 
    mileageAtInstallation = preferences.getString("mileage", ""); 
    batteryBrand = preferences.getString("batteryBrand", ""); 
    carColor = preferences.getString("carColor", ""); 
    mileageUnit = preferences.getString("mileageUnit", ""); 
    ISENROLLED = preferences.getBool("ISENROLLED",false);
    gender = preferences.getString("gender", ""); 
    batchNumber = preferences.getString("batchNumber", ""); 
    productType = preferences.getString("productType", ""); 
    transmissionType = preferences.getString("transmission", ""); 
    tyreBrand = preferences.getString("tyreBrand", ""); 
    dealerNumber = preferences.getString("dealerNumber", ""); 
    preferences.end();  

    (storedDevId != "") ? ISENROLLED = true: ISENROLLED = false;  // Set enrolled to 1 if device already registered and assigned an Id
    if(username != "" && email != ""  && telephone != ""  && telephone != "" )
    (ISENROLLED == 1 && IS_INITIALIZED == 1);
    delay(3000); // wait a bit 
    }
    //initialize Access POint
    if(!IS_INITIALIZED){
    accessPointInit(startAccessPoint);
    //SERVE HTML File
    serveHTML(startAccessPoint);
    }
    }
