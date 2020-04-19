#pragma once
#include <string>
#include <deque>
#include <ESP8266WebServer.h>
#include "OTAhandlers.h"

class pageHandlers
{
  public:
    pageHandlers();
    static void SetupWeb(pageHandlers& instance, OTAhandlers* OTAinstance);
    void handle();
    void ReadChar(const char c);
    const char* GetConsoleText();

  protected:
    void ResetRPi();
    static void handleRoot(pageHandlers& instance);
    static void handleConsole(pageHandlers& instance);
    static void handleConsText(pageHandlers& instance);
    static void handleSendCmd(pageHandlers& instance);
    static void handleLog(pageHandlers& instance);
    static void handleNotFound(pageHandlers& instance);

  private:
    OTAhandlers* pOTA;
    ESP8266WebServer server;
    std::string consoleLine;
    std::string consoleBuff;
    unsigned int lineCounter;
    static const unsigned int CONSOLE_MAX_LINES = 80;
    static const uint8_t RESET_PIN = D1;
};