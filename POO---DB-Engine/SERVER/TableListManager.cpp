#include "TableListManager.h"
#include "CurrentDatabaseManager.h"
#include <sstream>

TableListManager::TableListManager(const std::string& request) {
    // Extragem username și dbName din request: GET_TABLES:username:dbName
    size_t sep = request.find(':');
    size_t sep2 = request.find(':', sep + 1);
    username = request.substr(sep + 1, sep2 - sep - 1);
    dbName = request.substr(sep2 + 1);
}

std::string TableListManager::process() {
    try {
        CurrentDatabase* db = CurrentDatabaseManager::get();
        if (!db) return "TABLEDUMP:FAILED";

        std::ostringstream output;

        // Iterăm prin tabelele existente în memorie
        for (const auto& tableName : db->getTableNames()) {
            Table* t = db->getTable(tableName);
            if (!t) continue;

            output << "Table: " << tableName << "\n";

            for (const Column& col : t->getColumns()) {
                output << col.getName() << "\t";
            }
            output << "\n";

            for (const Row& row : t->getRows()) {
                for (const auto& val : row.getValues()) {
                    output << val << "\t";
                }
                output << "\n";
            }

            output << "\n";
        }

        return "TABLEDUMP:" + output.str();
    }
    catch (...) {
        return "TABLEDUMP:FAILED";
    }
}
