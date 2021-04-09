#pragma once
#include <string>

class statusLog
{
  public:
    static void writeMsg(const char* msg);
    static void writeLine(const char* msg);
    static const char* getStatus();

  private:
    static std::string logmsg;
    static const unsigned int STATUS_MAX_LINES = 50;
};
