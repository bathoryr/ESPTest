#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "OTAhandlers.h"
#include "pageHandlers.h"
#include "mqttClient.h"
#include "statusLog.h"

#ifndef STASSID
#error "No WiFi password defined! Set STASSID and STAPSK env. variables"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const uint8_t HEARTBEAT_PIN = D2;

OTAhandlers OTA;
pageHandlers Web;
mqttClient MQTT;

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
    statusLog::writeLine("Booting...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        statusLog::writeLine("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    statusLog::writeLine("Ready\nIP address: ");
    statusLog::writeLine(WiFi.localIP().toString().c_str());

    OTAhandlers::SetupOTA(OTA);
    pageHandlers::SetupWeb(Web);
    mqttClient::setup(MQTT, "lede0");

    // MDNS.begin("esp8266-a4ca23");

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(HEARTBEAT_PIN, INPUT);
    attachInterrupt(HEARTBEAT_PIN, gpio_change_handler, RISING);
}

unsigned long timeout10s = millis();

void loop()
{
    if (heartbeat_changed == true)
    {
        heartbeat_changed = false;
        last_heartbeat_change = millis();
        digitalWrite(LED_BUILTIN, LOW);
        dim_led = true;
        MQTT.publish("rpiguard/heartbeat", String(millis()).c_str());
    }
    if (dim_led == true && millis() - last_heartbeat_change > 20)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        dim_led = false;
    }

    if (millis() - timeout10s > 10000)
    {
        timeout10s = millis();
        auto alive = millis() - last_heartbeat_change;
        if (alive < 15000)
            MQTT.publish("rpiguard/status", "green");
        else
        if (alive >= 30000 && alive < 60000)
            MQTT.publish("rpiguard/status", "yellow");
        else
        if (alive >= 60000 && alive < 120000)
            MQTT.publish("rpiguard/status", "orange");
        else
            MQTT.publish("rpiguard/status", "red");
    }

    OTA.handle();
    Web.handle();
    MQTT.handle();
    // MDNS.update();
    while (Serial.available() > 0)
    {
        Web.ReadChar(Serial.read());
    }
}