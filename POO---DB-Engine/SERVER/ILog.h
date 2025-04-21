#pragma once
#include <string>

class ILog{
public:
	virtual ~ILog() = 0 {};
	virtual void showLog() const = 0;
	virtual void write(const std::string& message) = 0;
	virtual void erase() = 0;
};

