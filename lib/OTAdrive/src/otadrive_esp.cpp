#include "otadrive_esp.h"
#include "FlashUpdater.h"
#include "tinyHTTP.h"

#ifdef ESP32
#include <esp_task_wdt.h>
#endif

otadrive_ota OTADRIVE;
otadrive_ota::THandlerFunction_Progress otadrive_ota::_progress_callback = nullptr;

otadrive_ota::otadrive_ota()
{
#ifdef OTA_FILE_SYS
    setFileSystem(&OTA_FILE_SYS);
#endif
}

/**
 * Sets ApiKey and current firmware version of your device
 *
 */
void otadrive_ota::setInfo(String ApiKey, String Version)
{
    this->ProductKey = ApiKey;
    this->Version = Version;
}

void otadrive_ota::useSSL(bool ssl)
{
    _useSSL = ssl;
}

String otadrive_ota::serverUrl(String uri)
{
    String url = OTADRIVE_URL;
    url += uri;
    if (_useSSL)
        url.replace("http:", "https:");
    return url;
}
/**
 * Enable or disable MD5 compare strategy to decide download the new version or not
 *
 */
void otadrive_ota::useMD5Matcher(bool useMd5)
{
    MD5_Match = useMd5;
}

String otadrive_ota::baseParams()
{
    return "k=" + ProductKey + "&v=" + Version + "&s=" + getChipId();
}

/**
 * @brief Use this only if you sure to change the default chip uid. It could burn your device tickets on the OTAdrive.
 *
 * @param id Your unique id for this device
 */
void otadrive_ota::setChipId(String id)
{
    force_chipId = id;
}

/**
 * Returns your chip ID
 *
 * @return string of your unique chipID
 */
String otadrive_ota::getChipId()
{
    if (!force_chipId.isEmpty())
        return force_chipId;
#ifdef ESP8266
    return String(ESP.getChipId());
#elif defined(ESP32)
    String ChipIdHex = String((uint32_t)(ESP.getEfuseMac() >> 32), HEX);
    ChipIdHex += String((uint32_t)ESP.getEfuseMac(), HEX);
    return ChipIdHex;
#endif
}

String otadrive_ota::downloadResourceList(Client &client)
{
    String url = serverUrl("resource/list?plain&");
    url += baseParams();

    String res;
    download(client, url, nullptr, &res);
    return res;
}

bool otadrive_ota::download(Client &client, String url, File *file, String *outStr)
{
    OTAdrive_ns::TinyHTTP http(client, _useSSL);
    client.setTimeout(TIMEOUT_MS / 1000);
    http.user_headers = OTAdrive_ns::FlashUpdater::createHeaders();
    http.user_headers += "\r\nIf-None-Match: \"686897696a7c876b7e\"";

    if (http.get(url))
    {
        // httpCode will be negative on error
        if (http.resp_code == 200)
        {
            if (file)
            {
                unsigned int n = 0, rem = http.total_len;
                unsigned long t0 = millis();
                uint8_t wbuf[256];
                while (millis() - t0 < 15000 && rem > 0)
                {
                    int len = sizeof(wbuf);
                    if (len > rem)
                        len = rem;
                    if (http.client.available() < len)
                        len = http.client.available();
                    int rd = http.client.readBytes(wbuf, len);
                    n = file->write(wbuf, rd);
                    rem -= rd;
                    otd_log_v("%d/%d rem : %s", rem, http.total_len, file->name());
                }

                otd_log_i("%d/%d bytes downloaded and writted to file : %s", rem, http.total_len, file->name());
                file->close();
                return true;
            }

            if (outStr)
            {
                uint8_t wbuf[256];
                *outStr = "";
                while (http.client.available())
                {
                    int len = sizeof(wbuf) - 1;
                    if (http.client.available() < len)
                        len = http.client.available();

                    int rd = http.client.readBytes(wbuf, len);
                    wbuf[rd] = '\0';
                    *outStr += String((char *)wbuf);
                }
                return true;
            }
        }
        else
        {
            otd_log_e("downloaded error code %d, %s", http.resp_code, http.client.readString().c_str());
        }
    }

    return false;
}

/**
 * Call update API of the OTAdrive server and gets information about new firmware
 *
 * @return updateInfo object, contains information about new version on server
 */
void otadrive_ota::setFileSystem(FS *fileObj)
{
    this->fileObj = fileObj;
}

bool otadrive_ota::syncResources()
{
    WiFiClient client;
    return syncResources(client);
}
/**
 * Call resource API of the OTAdrive server and sync local files with server
 *
 * @return returns success if no error happens in the procedure
 */
bool otadrive_ota::syncResources(Client &client)
{
    if (fileObj == nullptr)
    {
        otd_log_e("file system doesn't set, call setFileSystem() in setup");
        return false;
    }

    String list = downloadResourceList(client);
    String baseurl = serverUrl("resource/get?");
    baseurl += baseParams();

    while (list.length())
    {
        // extract file info from webAPI
        String fn = cutLine(list);
        String fk = cutLine(list);
        String md5 = cutLine(list);
        if (!fn.startsWith("/"))
            fn = "/" + fn;

        otd_log_d("file data: %s, MD5=%s", fn.c_str(), md5.c_str());

        // check local file MD5 if exists
        String md5_local = "";
        if (fileObj->exists(fn))
        {
            File file = fileObj->open(fn, "r");
            md5_local = file_md5(file);
            file.close();
        }

        // compare local and server file checksum
        if (md5_local == md5)
        {
            otd_log_i("local MD5 is match for %s", fn.c_str());
            continue;
        }
        else
        {
            // lets download and replace file
            otd_log_i("MD5 not match for %s, lets download", fn.c_str());
            File file = fileObj->open(fn, "w+");
            if (!file)
            {
                otd_log_e("Faild to create file\n");
                return false;
            }

            download(client, baseurl + "&fk=" + fk, &file, nullptr);
            file.close();
        }
    }

    return true;
}

bool otadrive_ota::sendAlive()
{
    WiFiClient client;
    return sendAlive(client);
}
/**
 * Call alive API of the OTAdrive server and sends some device info
 *
 * @return returns success if no error happens in procedure
 */
bool otadrive_ota::sendAlive(Client &client)
{
    String url = serverUrl("alive?");
    url += baseParams();
    return download(client, url, nullptr, nullptr);
}

updateInfo otadrive_ota::updateFirmware(Client &client, bool reboot)
{
#ifdef ESP32
    esp_task_wdt_init(45, true);
#endif
    updateInfo inf = updateFirmwareInfo(client);
    if (!inf.available)
    {
        otd_log_i("No new firmware available");
        return inf;
    }

    String url = serverUrl("update?");
    url += baseParams();

    OTAdrive_ns::FlashUpdater updater;
    updater.MD5_Match = MD5_Match;
    updater.rebootOnUpdate(false);
    updater.onProgress(updateFirmwareProgress);
    FotaResult ret = updater.update(client, url);

    switch (ret)
    {
    case FOTA_UPDATE_FAILED:
        otd_log_i("HTTP_UPDATE_FAILED Error");
        break;

    case FOTA_UPDATE_NO_UPDATES:
        otd_log_i("HTTP_UPDATE_NO_UPDATES");
        break;

    case FOTA_UPDATE_OK:
    {
        Version = inf.version;
        sendAlive(client);
        otd_log_i("HTTP_UPDATE_OK");
        if (reboot)
            ESP.restart();
        break;
    }
    default:
        otd_log_i("HTTP_UPDATE_CODE: %d", ret);
        break;
    }

    return inf;
}

/**
 * Call update API of the OTAdrive server and download new firmware version if available
 * If new version download you never get out of this function. MCU will reboot
 */
updateInfo otadrive_ota::updateFirmware(bool reboot)
{
    WiFiClient client;
#ifdef ESP32
    esp_task_wdt_init(45, true);
#endif
    return updateFirmware(client, reboot);
}

/**
 * Set callback for onProgress during firmware update
 *
 */
void otadrive_ota::onUpdateFirmwareProgress(THandlerFunction_Progress fn)
{
    _progress_callback = fn;
}

void otadrive_ota::updateFirmwareProgress(int progress, int totalt)
{
#ifdef ESP32
    esp_task_wdt_reset();
#endif
    if (_progress_callback != nullptr)
        _progress_callback(progress, totalt);
}

updateInfo otadrive_ota::updateFirmwareInfo()
{
    WiFiClient client;
    return updateFirmwareInfo(client);
}
/**
 * Call update API of the OTAdrive server and gets information about new firmware
 *
 * @return updateInfo object, contains information about new version on server
 */
updateInfo otadrive_ota::updateFirmwareInfo(Client &client)
{
    updateInfo inf;
    String url = serverUrl("update?");
    url += baseParams();

    inf.available = false;
    inf.code = update_result::ConnectError;

    OTAdrive_ns::TinyHTTP http(client, _useSSL);

    OTAdrive_ns::FlashUpdater updater;
    updater.MD5_Match = MD5_Match;
    http.user_headers = OTAdrive_ns::FlashUpdater::createHeaders();

    if (!http.get(url, 0, 0))
    {
        otd_log_e("http error\n");
        return inf;
    }

    otd_log_i("heads [%d] \n", (int)http.resp_code);
    inf.version = http.xVersion;
    inf.size = http.total_len;

    switch (http.resp_code)
    {
    case 0:
        otd_log_e("connect error\n");
        inf.code = update_result::ConnectError;
        break;

    case 404:
        // HTTP_CODE_NOT_FOUND
        otd_log_i("not found error\n");
        inf.code = update_result::NoFirmwareExists;
        break;

    case 304:
        // HTTP_CODE_NOT_MODIFIED
        otd_log_i("no new firmware\n");
        inf.code = update_result::AlreadyUpToDate;
        break;

    case 401:
        // HTTP_CODE_UNAUTHORIZED
        otd_log_e("unauthorized error\n");
        inf.code = update_result::DeviceUnauthorized;
        break;

    case 200:
        // HTTP_CODE_OK
        inf.available = true;
        inf.code = update_result::Success;
        break;
    }
    client.stop();
    delay(100);

    return inf;
}

String otadrive_ota::getConfigs()
{
    return getJsonConfigs();
}

String otadrive_ota::getConfigs(Client &client)
{
    return getJsonConfigs(client);
}

String otadrive_ota::getJsonConfigs()
{
    WiFiClient client;
    return getJsonConfigs(client);
}
/**
 * Call configuration API of the OTAdrive server and gets device configuration as string
 *
 * @return configuration of device as String
 */
String otadrive_ota::getJsonConfigs(Client &client)
{
    String url = serverUrl("getconfig?");
    url += baseParams();

    String conf = "";
    download(client, url, nullptr, &conf);

    return conf;
}

OTAdrive_ns::KeyValueList otadrive_ota::getConfigValues()
{
    WiFiClient client;
    return getConfigValues(client);
}

OTAdrive_ns::KeyValueList otadrive_ota::getConfigValues(Client &client)
{
    String url = serverUrl("getconfig?plain&");
    url += baseParams();

    String conf = "";
    download(client, url, nullptr, &conf);

    otd_log_d("config download: %s\n", conf.c_str());

    OTAdrive_ns::KeyValueList list;
    list.load(conf);

    return list;
}

size_t updateInfo::printTo(Print &p) const
{
    String s = toString();
    return p.print(s);
}

String updateInfo::toString() const
{
    if (code == update_result::AlreadyUpToDate)
    {
        return String("Firmware already uptodate.\n");
    }
    else if (code == update_result::Success)
    {
        char t[64];
        sprintf(t, "Firmware update from %s to %s.\n", old_version.c_str(), version.c_str());
        return String(t);
    }
    if (code == update_result::DeviceUnauthorized)
    {
        return String("Device Unauthorized. Change the device state on OTAdrive.\n");
    }
    else
    {
        char t[32];
        sprintf(t, "OTA update Faild, %s.\n", code_str());
        return String(t);
    }
}

const char *updateInfo::code_str() const
{
    static const char *messages[] = {
        "Connect Error", "Device Unauthorized",
        "Already UpToDate", "No Firmware Exists",
        "Success", "Unkwon"};
    switch (code)
    {
    case update_result::Success:
        return messages[4];
    case update_result::ConnectError:
        return messages[0];
    case update_result::DeviceUnauthorized:
        return messages[1];
    case update_result::AlreadyUpToDate:
        return messages[2];
    case update_result::NoFirmwareExists:
        return messages[3];

    default:
        return messages[5];
        break;
    }
}

bool otadrive_ota::timeTick(uint16_t seconds)
{
    if (millis() > tickTimestamp)
    {
        tickTimestamp = millis() + ((uint32_t)seconds) * 1000;
        return true;
    }
    return false;
}