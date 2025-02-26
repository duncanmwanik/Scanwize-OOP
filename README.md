# SCANWIZE
![ScanWize](https://scanwize.io/_next/image?url=%2F_next%2Fstatic%2Fmedia%2Flogo.9d97e02a.png&w=256&q=75)

##  OOP Definitions

All class instances are defined in `main.cpp`.

1. `Device device`
Handles core functions specific to a device.
Interfaces with the automotive classes i.e. Toyota(), Mazda() etc.

2. `Modem modem`
Creates an internet connection and handles connection availability.

3. `Network network`
Handles all POST and GET requests.

4. `OTA ota`
Hadles OTA updates;

5. `AccessPoint accesspoint`
Creates the Wi-Fi accesspoint to initialize a device with user/vehicle data for the first time.

##  Operation Sequence
Starting from `main.cpp`

### In `Setup()` -> `device.start()`:

`isInitialized()` : check if device is initialized

  *if initialized:*
- `getStoredData()`- retreives saved user/vehicle data from prefs
- `modem.createInternetConnection()`- initialize modem internet connection
- `isRegistered()` and `network.registerDevice()`- register device if not registered
- `isEgineOn(true)`- checks engine status change and updates if there is change, also if it's in `setup()` (the boolean parameter indicated if it's in setup)

  *if not initialized:*
  - `accessPoint.start()`- create accesspoint to initialize device and on submission, data is saved to prefs and the device is restarted.

### In `Loop()` -> `device.readDiagnosticData()`

Runs only if device `isInitialized()` and `isRegistered()`

`isEgineOn()` checks engine status and posts only if there is a change in state

*if engine is on*:

For 10 times we read live data and send it to server `sendLiveData()`.

*`checkDTCs()`*:
- `network.getClearDTCStatus()`: checks if clearDTCDirective is true. If true, DTCs are cleared and ` network.postClearDTCStatus()` updates clearDTCDirective in the server.
- We read DTCs and send them to the server `sendDTCData(DTCData)`.

*if engine is off*:
 
 
`doReset()`: resets the device periodically to avoid crashes


##  Where to add Automotive Code

### `main.cpp`:
- Include the class i.e. `#include <ToyotaClass.h>` 
- Create the object i.e. `ToyotaClass Toyota = ToyotaClass();`

### `main.h`:
- Include the class i.e. `#include <ToyotaClass.h>`
- Extern the class instance i.e. `extern ToyotaClass Toyota;`.
 
### `device.cpp`:
##### `device.start()`:
- Initialize the object i.e. `Toyota->diagInit();`
- Check engine status i.e. `checkEngineStatus(Toyota->ecuOn());`.

##### `device.readDiagnosticData()`:
- Check engine status i.e. `checkEngineStatus(Toyota->ecuOn());`.
- Clear DTCs i.e. `Toyota->clearAllDTC(device.deviceId);`
- Check battery voltage: `checkBatteryVoltage(Toyota->batteryVoltageEcuOff(R1,R2,batPin));`

##### `device.readLiveData()`:
- Read live data i.e. `liveData = Toyota->...;`.
 
##### `device.readDTCData()`:
- Read DTC data i.e. `DTCData = Toyota->readAllEcuDtcs(storedDevId);`.
