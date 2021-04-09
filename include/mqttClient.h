#pragma once
#include <WiFiClient.h>
#include <PubSubClient.h>

class mqttClient
{
  public:
    mqttClient();
    static void setup(mqttClient& instance, const char* server, uint16_t port = 1883);
    void handle();
    bool publish(const char* topic, const char* payload, bool retain = false);

  private:
    void connect();
    WiFiClient wClient;
    PubSubClient mqtt;
    unsigned long last_attempt;
  
  protected:
    static void topic_callback(mqttClient& instance, char* ptopic, byte* payload, unsigned int length);
};