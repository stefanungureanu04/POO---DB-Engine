#include "DatabaseLoaderManager.h"

#include <filesystem>
#include <sstream>

DatabaseLoaderManager::DatabaseLoaderManager(const std::string& request) 
{
    size_t sep = request.find(':');
    size_t sep2 = request.find(':', sep + 1);
    username = request.substr(sep + 1, sep2 - sep - 1);
    dbName = request.substr(sep2 + 1);

    workingDatabase = nullptr;
}

std::string DatabaseLoaderManager::process() {

    try {
        workingDatabase = new Database(dbName);

        std::string folder = "databases/" + username + "/" + dbName + ".txt";

        workingDatabase->loadFromFile(folder);

        return "LOAD_DB_SUCCESS";

    }
    catch (...) {
        return "LOAD_DB_FAIL";
    }
}

Database* DatabaseLoaderManager::getDatabase() const
{
    return workingDatabase;
}
