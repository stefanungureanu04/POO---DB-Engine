#pragma once
#include "CLog.h"
#include <ctime>


class AppLog : public CLog{
private:
	static const std::string appLogFilename;

public:
	AppLog();
	void write(const std::string& message) override;
};

