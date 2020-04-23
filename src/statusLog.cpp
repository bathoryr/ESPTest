#include <algorithm>
#include "statusLog.h"

statusLog::statusLog()
{
}
    
void statusLog::writeLog(const char* msg)
{
    unsigned int lineCount = std::count(log.begin(), log.end(), '\n');
    while (lineCount >= STATUS_MAX_LINES)
    {
        log.erase(0, log.find_first_of('\n') + 1);
        lineCount = std::count(log.begin(), log.end(), '\n');
    }

    log += msg;
    if (log.back() != '\n')
    {
        log.append("\n");
    }
}

const char* statusLog::getStatus()
{
    return log.c_str();
}
