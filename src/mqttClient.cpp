#include <string>
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
            statusLog::writeLine("Connected to MQTT server");
            if (mqtt.subscribe("rpiguard/cmd"))
            {
                statusLog::writeLine("Topic subscribed");
            }
            if (mqtt.subscribe("rpiguard/config/serial"))
            {
                statusLog::writeLine("Topic subscribed");
            }
        }
        else
        {
            statusLog::writeLine("ERROR connecting to MQTT server");
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

bool mqttClient::publish(const char* topic, const char* payload, bool retain)
{
    return mqtt.publish(topic, payload, retain);
}

void mqttClient::topic_callback(mqttClient& instance, char* ptopic, byte* payload, unsigned int length)
{
    std::string topic(ptopic);
    std::string message = std::string((char*)payload, length);

    statusLog::writeMsg("MQTT message arrived: ");
    statusLog::writeLine(message.c_str());
    
    if (topic == "rpiguard/cmd" && message == "wakeup")
    {
        Serial.write('\n');
    }
    if (topic == "rpiguard/config/serial")
    {
        unsigned long baud = String(message.c_str()).toInt();
        Serial.begin(baud);
    }
}
