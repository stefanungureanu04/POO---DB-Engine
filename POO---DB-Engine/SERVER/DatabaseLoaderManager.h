#pragma once
#include "CurrentDatabase.h"
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>

class DatabaseLoaderManager {
private:
    std::string username;
    std::string dbName;

public:
    DatabaseLoaderManager(const std::string& request) {
        // Extragere user și db
        size_t sep = request.find(':');
        size_t sep2 = request.find(':', sep + 1);
        username = request.substr(sep + 1, sep2 - sep - 1);
        dbName = request.substr(sep2 + 1);
    }

    std::string process() {
        try {
            CurrentDatabase db(dbName, username);

            std::string folder = "databases/" + username + "/";
            for (const auto& entry : std::filesystem::directory_iterator(folder)) {
                std::string file = entry.path().filename().string();
                if (file.size() >= 4 && file.substr(file.size() - 4) == ".tbl" &&
                    file.find("_" + dbName + "_" + username) != std::string::npos) {
                    std::string tableName = file.substr(0, file.find('_'));
                    db.loadTable(tableName);
                }
            }

            // TODO: ai putea salva instanța `db` într-un singleton/global (cu atenție)
            return "LOAD_DB_SUCCESS";
        }
        catch (...) {
            return "LOAD_DB_FAIL";
        }
    }
};