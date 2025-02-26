#include "accesspoint.h"
#include "main.h"

AsyncWebServer server(80);

void AccessPoint::start()
{
    // Connect to Wi-Fi network with SSID and password
    logn("Creating Access Point...");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(WIFI_SSID, WIFI_PWD);
    delay(2000); // to avoid crash on WiFi Connection
    IPAddress IP = WiFi.softAPIP();

    logn(" at IP Address: http://");
    log(IP);

    serveHTML();
}

//
//

void handleJsonPost(AsyncWebServerRequest *request, JsonVariant &json)
{
    // Read the JSON data from the request body
    if (json.is<JsonObject>())
    {
        JsonObject jsonObject = json.as<JsonObject>();
        // Access the parsed JSON data
        String firstName = jsonObject["firstName"].as<String>();
        String lastName = jsonObject["lastName"].as<String>();
        String email = jsonObject["email"].as<String>();
        String gender = jsonObject["gender"].as<String>();
        String telephone = jsonObject["telephone"].as<String>();
        String whatsappTelephone = jsonObject["whatsappTelephone"].as<String>();
        String vehicleRegistrationNumber = jsonObject["vehicleRegistrationNumber"].as<String>();
        String batchNumber = jsonObject["batchNumber"].as<String>();
        String serialNumber = jsonObject["serialNumber"].as<String>();
        String vehicleMake = jsonObject["vehicleMake"].as<String>();
        String model = jsonObject["model"].as<String>();
        String yearOfMake = jsonObject["yearOfMake"].as<String>();
        String productType = jsonObject["productType"].as<String>();
        String transmissionType = jsonObject["transmissionType"].as<String>();
        String chasisNumber = jsonObject["chasisNumber"].as<String>();
        String fuelType = jsonObject["fuelType"].as<String>();
        String engineCapacity = jsonObject["engineCapacity"].as<String>();
        String engineCode = jsonObject["engineCode"].as<String>();
        String mileageAtInstallation = jsonObject["mileageAtInstallation"].as<String>();
        String batteryBrand = jsonObject["batteryBrand"].as<String>();
        String carColor = jsonObject["carColor"].as<String>();
        String mileageUnit = jsonObject["mileageUnit"].as<String>();
        String tyreBrand = jsonObject["tyreBrand"].as<String>();
        String dealerNumber = jsonObject["dealerNumber"].as<String>();
        String networkMode = jsonObject["networkMode"].as<String>();

        // Save the data to Preferences
        preferences.begin("ScanMax", false);
        preferences.putString("firstName", firstName);
        preferences.putString("lastName", lastName);
        preferences.putString("email", email);
        preferences.putString("gender", gender);
        preferences.putString("telephone", telephone);
        preferences.putString("batchNumber", batchNumber);
        preferences.putString("serialNumber", serialNumber);
        preferences.putString("WhatsappNo", whatsappTelephone);
        preferences.putString("vehicleReg", vehicleRegistrationNumber);
        preferences.putString("vehicleMake", vehicleMake);
        preferences.putString("model", model);
        preferences.putString("yearOfMake", yearOfMake);
        preferences.putString("productType", productType);
        preferences.putString("transmission", transmissionType);
        preferences.putString("chasisNumber", chasisNumber);
        preferences.putString("tyreBrand", tyreBrand);
        preferences.putString("fuelType", fuelType);
        preferences.putString("engineCapacity", engineCapacity);
        preferences.putString("engineCode", engineCode);
        preferences.putString("mileage", mileageAtInstallation);
        preferences.putString("batteryBrand", batteryBrand);
        preferences.putString("carColor", carColor);
        preferences.putString("dealerNumber", dealerNumber);
        preferences.putString("mileageUnit", mileageUnit);
        preferences.putString("networkMode", networkMode);
        preferences.end();

        // update isInitialized
        preferences.begin("ScanMax", false);
        preferences.putBool("isInitialized", true);
        preferences.end();

        // Send a response
        request->send(200, "text/plain", "Data received successfully");

        server.end();
        WiFi.disconnect();
        WiFi.softAPdisconnect();

        log("Device initialized...\n");
    }
    else
    {
        // Failed to parse JSON
        log("Invalid Json Submitted!");
        request->send(400, "text/plain", "Invalid JSON data");
    }
}

//
//

void AccessPoint::serveHTML()
{
    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        log("An error occurred while mounting SPIFFS");
        return;
    }

    log("SPIFFS mounted...");

    // Start the server
    server.begin();

    // Serve the HTML, CSS, and JS files
    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(404, "text/plain", "Not found"); });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/style.css", "text/css"); });
    server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/logo.png", "image/png"); });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/script.js", "application/javascript"); });

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler(
        "/submit", [](AsyncWebServerRequest *request, JsonVariant &json)
        { handleJsonPost(request, json); });

    server.addHandler(handler);

    log("Accesspoint is live...");
}

//
//
