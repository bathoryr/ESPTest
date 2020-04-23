#pragma once
#include <WiFiClient.h>
#include <PubSubClient.h>

class mqttClient
{
  public:
    mqttClient();
    static void setup(mqttClient& instance, const char* server, uint16_t port = 1883);
    void handle();

  private:
    void connect();
    WiFiClient wClient;
    PubSubClient mqtt;
    unsigned long last_attempt;
  
  protected:
    static void topic_callback(mqttClient& instance, char* topic, byte* payload, unsigned int length);
};