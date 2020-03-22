#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "consolePage.h"

#ifndef STASSID
  #error "No WiFi password defined! Set STASSID and STAPSK env. variables"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);
String consoleBuf;
String statusLog;
const unsigned int CONSOLE_BUF_LEN = 10240;
const unsigned int STATUS_BUF_LEN = 2048;

void handleRoot() {
  digitalWrite(LED_BUILTIN, 1);
  String msg = "<h1>ESP8266 web server</h1>\n\n";
  msg += "IP address: " + WiFi.localIP().toString() + "<br/>";
  msg += "<p><a href=\"/console\">RPi console output</a></p>";
  msg += "<p><a href=\"/console2\">RPi console output 2</a></p>";
  msg += "<p><a href=\"/log\">ESP status log</a></p>";
  server.send(200, "text/html", msg);
  digitalWrite(LED_BUILTIN, 0);
}

void handleConsole2()
{
  server.send(200, "text/html", console_page);
}

void handleConsText()
{
  server.send(200, "text/plain", consoleBuf);
}

void handleSendCmd()
{
  if (server.method() == HTTP_GET)
  {
    // Send data entered into command line
    if (server.arg("cmd").length() > 0)
    {
      Serial.write((server.arg("cmd") + '\r').c_str());
    }
  }
}

void handleConsole()
{
  if (server.method() == HTTP_POST)
  {
    // Send data entered into command line
    if (server.arg("cmd").length() > 0)
    {
      Serial.write((server.arg("cmd") + '\r').c_str());
      yield();
    }
  }
  String s = "<p><pre>" + consoleBuf + "</pre></p><hr/><p><form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/console\">\
      <input type=\"text\" name=\"cmd\" value=\"world\"><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form></p>";

  server.send(200, "text/html", s);
}

void handleLog()
{
  server.send(200, "text/plain", statusLog);
}

void handleNotFound(){
  digitalWrite(LED_BUILTIN, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, 0);
}

void setup()
{
  Serial.begin(115200);
  consoleBuf.reserve(CONSOLE_BUF_LEN);
  statusLog.reserve(STATUS_BUF_LEN);
  statusLog += "Booting\n";
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    statusLog += "Connection Failed! Rebooting...\n";
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
    {
      type = "sketch";
    }
    else
    { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    statusLog += "Start updating " + type;
  });
  ArduinoOTA.onEnd([]() {
    statusLog += "\nEnd";
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    char szbuf[128];
    snprintf(szbuf, 127, "Progress: %u%%\r", (progress / (total / 100)));
    statusLog += szbuf;
  });
  ArduinoOTA.onError([](ota_error_t error) {
    char szbuf[128];
    snprintf(szbuf, 127, "Error[%u]: ", error);
    statusLog += szbuf;
    if (error == OTA_AUTH_ERROR)
    {
      statusLog += "Auth Failed\n";
    }
    else if (error == OTA_BEGIN_ERROR)
    {
      statusLog += "Begin Failed\n";
    }
    else if (error == OTA_CONNECT_ERROR)
    {
      statusLog += "Connect Failed\n";
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
      statusLog += "Receive Failed\n";
    }
    else if (error == OTA_END_ERROR)
    {
      statusLog += "End Failed\n";
    }
  });
  ArduinoOTA.begin();
  statusLog += "Ready\nIP address: ";
  statusLog += WiFi.localIP().toString();

  server.on("/", handleRoot);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/console", handleConsole);
  server.on("/log", handleLog);
  server.onNotFound(handleNotFound);
  server.on("/console2", handleConsole2);
  server.on("/consoleText", handleConsText);
  server.on("/sendCmd", handleSendCmd);
  // MDNS.begin("esp8266-a4ca23");
  server.begin();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();
  // MDNS.update();
  while (Serial.available() > 0)
  {
    consoleBuf += (char)Serial.read();
  }

  auto tenths = millis() / 100 % 10;
  if (tenths == 0)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (tenths == 1)
  {
    digitalWrite(LED_BUILTIN, HIGH);

    // truncate string buffers
    if (consoleBuf.length() > CONSOLE_BUF_LEN)
    {
      consoleBuf.remove(CONSOLE_BUF_LEN);
    }
    if (statusLog.length() > STATUS_BUF_LEN)
    {
      statusLog.remove(STATUS_BUF_LEN);
    }
  }
}