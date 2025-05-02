#include "TableListManager.h"

#include <sstream>

TableListManager::TableListManager(const std::string& request, Database* database)
{
    size_t sep = request.find(':');
    size_t sep2 = request.find(':', sep + 1);
    username = request.substr(sep + 1, sep2 - sep - 1);
    dbName = request.substr(sep2 + 1);

    workingDatabase = database;
}

std::string TableListManager::process() {
    
    try {

        if (!workingDatabase) {
            return "TABLEDUMP:FAILED!";
        }

        std::string output{};

        output.clear();

        for (const auto& tableName : workingDatabase->getTableNames()) {
            output += tableName + "\n";
        }

        if (output.empty()) {
            return "TABLEDUMP:EMPTY";
        }

        return "TABLEDUMP:" + output;
    }
    catch (...) {
        return "TABLEDUMP:FAILED";
    }
}
