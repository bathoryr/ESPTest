#include "pageHandlers.h"
#include "consolePage.h"

pageHandlers::pageHandlers()
{
    server.begin(80);
    consoleLine.reserve(80);
    lineCounter = 0;
    pOTA = nullptr;
}

void pageHandlers::handle()
{
    server.handleClient();
}

void pageHandlers::ReadChar(const char c)
{
    consoleLine += c;
    if (c == '\n')
    {
        lineCounter++;
        while (lineCounter >= CONSOLE_MAX_LINES)
        {
            consoleBuff.erase(0, consoleBuff.find_first_of('\n') + 1);
            lineCounter--;
        }

        consoleBuff += consoleLine;
        consoleLine.clear();
    }
}

const char* pageHandlers::GetConsoleText()
{
    return consoleBuff.c_str();
}

void pageHandlers::SetupWeb(pageHandlers& instance, OTAhandlers* OTAinstance)
{
    instance.pOTA = OTAinstance;

    instance.server.on("/", [&instance](){handleRoot(instance);});
    instance.server.on("/inline", [&instance]() {
        instance.server.send(200, "text/plain", "this works as well");
    });
    instance.server.on("/log", [&instance](){handleLog(instance);});
    instance.server.onNotFound([&instance](){handleNotFound(instance);});
    instance.server.on("/console", [&instance](){handleConsole(instance);});
    instance.server.on("/consoleText", [&instance](){handleConsText(instance);});
    instance.server.on("/sendCmd", [&instance](){handleSendCmd(instance);});
    instance.server.begin();
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
                digitalWrite(D1, LOW);
                pinMode(D1, OUTPUT);
                delay(20);
                pinMode(D1, INPUT);
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
    instance.server.send(200, "text/plain", instance.pOTA != nullptr ? instance.pOTA->GetStatus() : "--");
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
