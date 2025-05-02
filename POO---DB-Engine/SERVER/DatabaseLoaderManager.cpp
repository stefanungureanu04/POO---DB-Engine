#include "DatabaseLoaderManager.h"
#include "CurrentDatabase.h"
#include "CurrentDatabaseManager.h"
#include <filesystem>
#include <iostream>

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

        if (!fs::exists(folder)) {
            std::cerr << "Folder does not exist: " << folder << "\n";
            return "LOAD_DB_FAIL";
        }

        int loadedCount = 0;

        for (const auto& entry : fs::directory_iterator(folder)) {
            std::string file = entry.path().filename().string();
            std::string suffix = "_" + dbName + "_" + username + ".tbl";

            // Verificăm dacă fișierul respectă convenția
            if (file.size() > suffix.size() &&
                file.substr(file.size() - suffix.size()) == suffix) {

                std::string tableName = file.substr(0, file.size() - suffix.size());

                std::cout << "[DBLoader] Loading table: " << tableName << std::endl;

                db->loadTable(tableName);
                ++loadedCount;
            }
        }

        // Dacă nu s-a încărcat nimic, raportăm
        if (loadedCount == 0) {
            std::cerr << "[DBLoader] No tables loaded. Possibly missing or misnamed files.\n";
            return "LOAD_DB_FAIL";
        }

        // Setăm în managerul global
        CurrentDatabaseManager::set(std::move(db));
        return "LOAD_DB_SUCCESS";
    }
    catch (const std::exception& e) {
        std::cerr << "[DBLoader] Exception: " << e.what() << "\n";
        return "LOAD_DB_FAIL";
    }
}
