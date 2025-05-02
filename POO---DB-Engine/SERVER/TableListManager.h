#pragma once
#include "Database.h"
#include <string>

class TableListManager {
private:
    std::string username;
    std::string dbName;
    Database* workingDatabase = nullptr;

public:
    TableListManager(const std::string& request, Database* database);
    std::string process();
};
