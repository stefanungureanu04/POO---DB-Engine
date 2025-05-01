#pragma once
#include <string>

class DatabaseSelectManager{
private:
	std::string request;

public:
	DatabaseSelectManager(const std::string& request);
	const std::string processDatabaseRequest();
};

