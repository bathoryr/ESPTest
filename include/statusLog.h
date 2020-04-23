#pragma once
#include <string>

class statusLog
{
  private:
    statusLog();

  public:
    void writeLog(const char* msg);
    const char* getStatus();

    static statusLog& instance()
    {
        static statusLog INST;
        return INST;
    }

  private:
    std::string log;
    static const unsigned int STATUS_MAX_LINES = 50;
};