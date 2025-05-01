#pragma once
#include <string>

class QueryManager{
private:
	std::string request;

public:
	QueryManager(const std::string& request);
	const std::string processQueryRequest();
};

