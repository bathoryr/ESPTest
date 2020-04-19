#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "OTAhandlers.h"
#include "pageHandlers.h"

#ifndef STASSID
#error "No WiFi password defined! Set STASSID and STAPSK env. variables"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

OTAhandlers OTA;
pageHandlers Web;

void setup()
{
    Serial.begin(115200);
    OTA.WriteStatus("Booting\n");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        OTA.WriteStatus("Connection Failed! Rebooting...\n");
        delay(5000);
        ESP.restart();
    }
    OTA.WriteStatus("Ready\nIP address: ");
    OTA.WriteStatus(WiFi.localIP().toString());

    OTAhandlers::SetupOTA(OTA);
    pageHandlers::SetupWeb(Web, &OTA);

    // MDNS.begin("esp8266-a4ca23");

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    OTA.handle();
    Web.handle();
    // MDNS.update();
    while (Serial.available() > 0)
    {
        Web.ReadChar(Serial.read());
    }

    auto tenths = millis() / 100 % 10;
    if (tenths == 0)
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
    else if (tenths == 1)
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
}