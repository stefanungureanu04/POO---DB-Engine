#pragma once
#include "UserCredentialsDatabase.h"
#include <string>

class AuthenticationHandler {
public:
    std::string handle(const std::string& request);
};
