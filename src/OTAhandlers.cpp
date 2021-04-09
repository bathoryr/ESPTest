#include <OTAhandlers.h>
#include "statusLog.h"

OTAhandlers::OTAhandlers()
{
}

void OTAhandlers::SetupOTA(OTAhandlers& instance)
{
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    instance.OTA.onStart([&instance](){onStart(instance);});
    instance.OTA.onEnd([&instance](){onEnd(instance);});
    instance.OTA.onProgress([&instance](size_t progress, size_t total){onProgress(instance, progress, total);});
    instance.OTA.onError([&instance](ota_error_t error){onError(instance, error);});
    instance.OTA.begin();
}

void OTAhandlers::handle()
{
    OTA.handle();
}

void OTAhandlers::onStart(OTAhandlers& instance)
{
    String type;
    if (instance.OTA.getCommand() == U_FLASH)
    {
        type = "sketch";
    }
    else
    { // U_FS
        type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    statusLog::writeLine(("Start updating " + type).c_str());
}

void OTAhandlers::onProgress(OTAhandlers& instance, unsigned int progress, unsigned int total) 
{
    char szbuf[128];
    snprintf(szbuf, 127, "Progress: %u%%\r", (progress / (total / 100)));
    statusLog::writeLine(szbuf);
}
void OTAhandlers::onEnd(OTAhandlers& instance) 
{
    statusLog::writeLine("End");
}

void OTAhandlers::onError(OTAhandlers& instance, ota_error_t error) 
{
    char szbuf[128];
    snprintf(szbuf, 127, "Error[%u]: ", error);
    statusLog::writeMsg(szbuf);
    if (error == OTA_AUTH_ERROR)
    {
      statusLog::writeLine("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR)
    {
      statusLog::writeLine("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR)
    {
      statusLog::writeLine("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
      statusLog::writeLine("Receive Failed");
    }
    else if (error == OTA_END_ERROR)
    {
      statusLog::writeLine("End Failed");
    }
}
