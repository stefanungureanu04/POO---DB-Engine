#pragma once
#include <string>
#include "Database.h"

class TriggersManager {
public:
    TriggersManager(const std::string& request, Database* db);
    std::string process();

private:
    std::string username;
    std::string dbName;
    Database* workingDb;
};
