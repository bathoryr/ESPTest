#include <algorithm>
#include "statusLog.h"

std::string statusLog::logmsg;

void statusLog::writeMsg(const char* msg)
{
    logmsg += msg;
}

void statusLog::writeLine(const char* msg)
{
    unsigned int lineCount = std::count(logmsg.begin(), logmsg.end(), '\n');
    while (lineCount >= STATUS_MAX_LINES)
    {
        logmsg.erase(0, logmsg.find_first_of('\n') + 1);
        lineCount = std::count(logmsg.begin(), logmsg.end(), '\n');
    }

    logmsg += msg;
    if (logmsg.back() != '\n')
    {
        logmsg.append("\n");
    }
}

const char* statusLog::getStatus()
{
    return logmsg.c_str();
}
