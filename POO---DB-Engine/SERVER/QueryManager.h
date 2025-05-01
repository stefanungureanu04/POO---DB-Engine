#pragma once
#include <string>
#include <filesystem>

class QueryManager{
private:
	std::string request;

public:
	QueryManager(const std::string& request);
	const std::string processQueryRequest();
};

