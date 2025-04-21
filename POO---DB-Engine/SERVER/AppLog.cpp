#define _CRT_SECURE_NO_WARNINGS
#include "AppLog.h"

const std::string AppLog::appLogFilename = "log.txt";

AppLog::AppLog()
{
	this->filename = AppLog::appLogFilename;
}

void AppLog::write(const std::string& message)
{
	std::ofstream file(AppLog::filename, std::ios::app);

    if (!file.is_open()) return;

    std::time_t now = std::time(nullptr);
    char timeStr[100];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::string timestamp(timeStr);
    file << "[" << timestamp << "]: " << message << "\n";
}
