#pragma once
#include "Database.h"
#include <string>

class DatabaseLoaderManager {
private:
    std::string username;
    std::string dbName;
    Database* workingDatabase;

public:
    DatabaseLoaderManager(const std::string& request);
    std::string process();
    Database* getDatabase() const;
};