#pragma once
#include "CLog.h"

class CommandHistoryLog : public CLog{
public:
	CommandHistoryLog(const std::string& filename);
	void write(const std::string& message) override;
};

