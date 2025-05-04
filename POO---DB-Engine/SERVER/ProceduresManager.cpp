#include "ProceduresManager.h"

ProceduresManager::ProceduresManager(const std::string& request, Database* database)
{
    size_t sep1 = request.find(':');
    size_t sep2 = request.find(':', sep1 + 1);
    username = request.substr(sep1 + 1, sep2 - sep1 - 1);
    dbName = request.substr(sep2 + 1);

    workingDatabase = database;
}

std::string ProceduresManager::process()
{
    try {
        if (!workingDatabase) {
            return "PROCEDURES:FAILED_TO_LOAD_DB";
        }

        std::string relations = workingDatabase->getProceduresInfo();
        if (relations.empty()) {
            return "PROCEDURES:NONE";
        }

        return "PROCEDURES:" + relations;
    }
    catch (const std::exception& e) {
        return "PROCEDURES:EXCEPTION";
    }
}
