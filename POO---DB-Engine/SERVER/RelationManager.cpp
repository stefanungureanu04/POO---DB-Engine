#include "RelationManager.h"
#include "Database.h"
#include <filesystem>
#include <sstream>
#include <iostream>

RelationManager::RelationManager(const std::string& request) {
    size_t sep1 = request.find(':');
    size_t sep2 = request.find(':', sep1 + 1);

    username = request.substr(sep1 + 1, sep2 - sep1 - 1);
    dbName = request.substr(sep2 + 1);
}

std::string RelationManager::process() {
    try {
        std::string dbPath = "databases/" + username + "/" + dbName + ".txt";

        Database db(dbName);
        if (!db.loadFromFile(dbPath)) {
            return "RELATIONS:FAILED_TO_LOAD_DB";
        }

        std::string relations = db.getRelationsAsString();
        if (relations.empty()) {
            return "RELATIONS:NONE";
        }

        return "RELATIONS:" + relations;
    }
    catch (const std::exception& e) {
        std::cerr << "[RelationManager] Exception: " << e.what() << "\n";
        return "RELATIONS:EXCEPTION";
    }
}