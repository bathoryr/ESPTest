#pragma once
#include <Arduino.h>
#define NO_GLOBAL_ARDUINOOTA
#include <ArduinoOTA.h>

class OTAhandlers
{
  public:
    OTAhandlers();
    static void SetupOTA(OTAhandlers& instance);
    void handle();
  
  protected:
    static void onStart(OTAhandlers& instance);
    static void onProgress(OTAhandlers& instance, unsigned int progress, unsigned int total);
    static void onEnd(OTAhandlers& instance);
    static void onError(OTAhandlers& instance, ota_error_t error) ;

  private:
    ArduinoOTAClass OTA;
};