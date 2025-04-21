#pragma once
#include "CLog.h"

class SessionLog : public CLog {
public:
	SessionLog(const std::string& filename);
	void write(const std::string& message) override;
};

