#pragma once
#include "UserCredentialsDatabase.h"
#include <string>

class AuthentificationHandler {
public:
    std::string handle(const std::string& request);
};
