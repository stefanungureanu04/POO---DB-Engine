#pragma once
#include "Database.h"
#include <string>

class RelationManager {
private:
    std::string username;
    std::string dbName;
    Database* workingDatabase;

public:
    RelationManager(const std::string& request, Database* database);
    std::string process();
};