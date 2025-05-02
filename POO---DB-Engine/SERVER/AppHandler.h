#pragma once
#include "Database.h"
#include <string>

class AppHandler {
private:
    Database *workingDatabase = nullptr;

public:
    std::string handle(const std::string& request);

private:
   bool beginsWith(const std::string& request, const std::string& prefix);
};
