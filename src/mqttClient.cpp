#include <ESP8266WiFi.h>
#include "statusLog.h"
#include "mqttClient.h"

mqttClient::mqttClient() : mqtt(wClient), last_attempt(0l)
{
}

void mqttClient::setup(mqttClient& instance, const char* server, uint16_t port)
{
    instance.mqtt.setServer(server, port);
    instance.mqtt.setCallback([&instance](char* topic, byte* payload, unsigned int length){mqttClient::topic_callback(instance, topic, payload, length);});
}
void mqttClient::connect()
{
    if (!mqtt.connected())
    {
        if (mqtt.connect(WiFi.hostname().c_str()))
        {
            mqtt.subscribe("RPiGuard/cmd");
        }
        else
        {
            statusLog::instance().writeLog("ERROR connecting to MQTT server");
        }
        last_attempt = millis();       
    }
}

void mqttClient::handle()
{
    if (!mqtt.connected() && (millis() - last_attempt > 5000))
    {
        connect();
    }
    mqtt.loop();
}

void mqttClient::topic_callback(mqttClient& instance, char* topic, byte* payload, unsigned int length)
{

}
