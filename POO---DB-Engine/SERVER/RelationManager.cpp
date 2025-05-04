#include "RelationManager.h"
#include <filesystem>
#include <sstream>
#include <iostream>

RelationManager::RelationManager(const std::string& request, Database* database) 
{
    size_t sep1 = request.find(':');
    size_t sep2 = request.find(':', sep1 + 1);
    username = request.substr(sep1 + 1, sep2 - sep1 - 1);
    dbName = request.substr(sep2 + 1);

    workingDatabase = database; 
}


std::string RelationManager::process() {
    try {
        if (!workingDatabase) {
            return "RELATIONS:FAILED_TO_LOAD_DB";
        }

        std::string relations = workingDatabase->getRelationsAsString();
        if (relations.empty()) {
            return "RELATIONS:NONE";
        }

        return "RELATIONS:" + relations;
    }
    catch (const std::exception& e) {
        return "RELATIONS:EXCEPTION";
    }
}