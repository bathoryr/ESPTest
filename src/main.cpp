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
const uint8_t HEARTBEAT_PIN = D2;

OTAhandlers OTA;
pageHandlers Web;

unsigned long last_heartbeat_change = 0ul;
volatile bool heartbeat_changed;
bool dim_led;

ICACHE_RAM_ATTR void gpio_change_handler()
{
    heartbeat_changed = true;
}

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
    pinMode(HEARTBEAT_PIN, INPUT);
    attachInterrupt(HEARTBEAT_PIN, gpio_change_handler, RISING);
}

void loop()
{
    if (heartbeat_changed == true)
    {
        heartbeat_changed = false;
        last_heartbeat_change = millis();
        digitalWrite(LED_BUILTIN, LOW);
        dim_led = true;
    }
    if (dim_led == true && millis() - last_heartbeat_change > 20)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        dim_led = false;
    }

    OTA.handle();
    Web.handle();
    // MDNS.update();
    while (Serial.available() > 0)
    {
        Web.ReadChar(Serial.read());
    }
}