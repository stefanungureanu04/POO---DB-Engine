#pragma once
#include <string>

class AuthenticationManager{
private:
	std::string request;

public:
	AuthenticationManager(const std::string& request);
	const std::string processAuthentication();
};

