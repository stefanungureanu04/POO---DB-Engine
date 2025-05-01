#pragma once
#include <string>

class LogProviderManager{
private:
	std::string request;

public:
	LogProviderManager(const std::string& request);
	const std::string processLogRequests();
};

