#pragma once
#include <string>
#include "Database.h"


class ProceduresManager {
private:
    std::string username;
    std::string dbName;
    Database* workingDatabase;

public:
    ProceduresManager(const std::string& request, Database* database);
    std::string process();
};

