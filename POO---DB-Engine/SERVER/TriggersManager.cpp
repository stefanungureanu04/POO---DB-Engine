#include "TriggersManager.h"
#include <sstream>

TriggersManager::TriggersManager(const std::string& request, Database* db)
{
    workingDb = db;
    size_t colon1 = request.find(':');
    size_t colon2 = request.find(':', colon1 + 1);

    if (colon1 != std::string::npos && colon2 != std::string::npos) {
        username = request.substr(colon1 + 1, colon2 - colon1 - 1);
        dbName = request.substr(colon2 + 1);
    }
}

std::string TriggersManager::process()
{
    if (username.empty() || dbName.empty())
        return "TRIGGERS_FAIL: invalid format";

    Database* db = Database::loadDatabaseForUser(username, dbName);
    if (!db)
        return "TRIGGERS_FAIL: database not found";

    std::string info = db->getTriggersInfo();
    delete db;
    return "TRIGGERS:" + info;
}
