#include "pageHandlers.h"
#include "consolePage.h"
#include "statusLog.h"
#include "logPage.h"

pageHandlers::pageHandlers()
{
    server.begin(80);
    consoleBuff.reserve(80 * 100);
}

void pageHandlers::handle()
{
    server.handleClient();
}

void pageHandlers::ReadChar(const char c)
{
    consoleBuff += c;
    if (c == '\n')
    {
        unsigned int lineCount = std::count(consoleBuff.begin(), consoleBuff.end(), '\n');
        while (lineCount >= CONSOLE_MAX_LINES)
        {
            consoleBuff.erase(0, consoleBuff.find_first_of('\n') + 1);
            lineCount = std::count(consoleBuff.begin(), consoleBuff.end(), '\n');
        }
    }
}

const char* pageHandlers::GetConsoleText()
{
    return consoleBuff.c_str();
}

void pageHandlers::SetupWeb(pageHandlers& instance)
{
    pinMode(RESET_PIN, INPUT);

    instance.server.on("/", [&instance](){handleRoot(instance);});
    instance.server.on("/log", [&instance](){handleLog(instance);});
    instance.server.on("/logText", [&instance](){handleLogText(instance);});
    instance.server.on("/console", [&instance](){handleConsole(instance);});
    instance.server.on("/consoleText", [&instance](){handleConsText(instance);});
    instance.server.on("/sendCmd", [&instance](){handleSendCmd(instance);});
    instance.server.onNotFound([&instance](){handleNotFound(instance);});

    instance.server.begin();
}

void pageHandlers::ResetRPi()
{
    digitalWrite(RESET_PIN, LOW);
    pinMode(RESET_PIN, OUTPUT);
    delay(20);
    pinMode(RESET_PIN, INPUT);
}

void pageHandlers::handleRoot(pageHandlers& instance)
{
    String msg = "<h1>ESP8266 web server</h1>\n\n";
    msg += "IP address: " + WiFi.localIP().toString() + "<br/>";
    msg += "<p><a href=\"/console\">RPi console output</a></p>";
    msg += "<p><a href=\"/log\">ESP status log</a></p>";
    instance.server.send(200, "text/html", msg);
}

void pageHandlers::handleSendCmd(pageHandlers& instance)
{
    if (instance.server.method() == HTTP_GET)
    {
        // Send data entered into command line
        if (instance.server.arg("cmd").length() > 0)
        {
            if (instance.server.arg("cmd") == "RESET")
            {
                instance.ResetRPi();
            }
            else if (instance.server.arg("cmd") == "CLRSCR")
            {
                instance.consoleBuff.clear();
            }
            else
            {
                Serial.write((instance.server.arg("cmd") + '\r').c_str());
            }

            instance.server.send(200, "text/plain", "OK");
        }
        else
        {
            Serial.write("\r\n");
        }
    }
}

void pageHandlers::handleConsole(pageHandlers& instance)
{
    instance.server.send(200, "text/html", console_page);
}

void pageHandlers::handleConsText(pageHandlers& instance)
{
    instance.server.send(200, "text/plain", instance.consoleBuff.c_str());
}

void pageHandlers::handleLog(pageHandlers& instance)
{
    instance.server.send(200, "text/html", log_page);
}

void pageHandlers::handleLogText(pageHandlers& instance)
{
    instance.server.send(200, "text/plain", statusLog::getStatus());
}

void pageHandlers::handleNotFound(pageHandlers& instance)
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += instance.server.uri();
    message += "\nMethod: ";
    message += (instance.server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += instance.server.args();
    message += "\n";
    for (uint8_t i = 0; i < instance.server.args(); i++)
    {
        message += " " + instance.server.argName(i) + ": " + instance.server.arg(i) + "\n";
    }
    instance.server.send(404, "text/plain", message);
}
