#pragma once
#include <string>
#include "Database.h"

class RelationManager {
private:
    std::string username;
    std::string dbName;
    Database* workingDatabase;

public:
    RelationManager(const std::string& request, Database* database);
    std::string process();
};