#include "DatabaseLoaderManager.h"
#include "CurrentDatabase.h"
#include "CurrentDatabaseManager.h"
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

DatabaseLoaderManager::DatabaseLoaderManager(const std::string& request) {
    size_t sep = request.find(':');
    size_t sep2 = request.find(':', sep + 1);
    username = request.substr(sep + 1, sep2 - sep - 1);
    dbName = request.substr(sep2 + 1);
}

std::string DatabaseLoaderManager::process() {
    try {
        auto db = std::make_unique<CurrentDatabase>(dbName, username);

        std::string folder = "databases/" + username + "/";
        for (const auto& entry : fs::directory_iterator(folder)) {
            std::string file = entry.path().filename().string();

            std::string expectedSuffix = "_" + dbName + "_" + username + ".tbl";
            if (file.size() >= expectedSuffix.size() &&
                file.substr(file.size() - expectedSuffix.size()) == expectedSuffix) {

                std::string tableName = file.substr(0, file.size() - expectedSuffix.size());
                db->loadTable(tableName);
            }
        }

        CurrentDatabaseManager::set(std::move(db));
        return "LOAD_DB_SUCCESS";

        

    }
    catch (...) {
        return "LOAD_DB_FAIL";
    }
}