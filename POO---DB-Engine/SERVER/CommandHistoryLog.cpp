#include "CommandHistoryLog.h"

CommandHistoryLog::CommandHistoryLog(const std::string& filename) : CLog(filename) {}

void CommandHistoryLog::write(const std::string& message)
{
    std::ofstream file(this->filename, std::ios::app);

    if (!file.is_open()) return;

    file << message << "\n";
}


