#include "DatabaseSelectManager.h"
#include "AppLog.h"
#include <fstream>

DatabaseSelectManager::DatabaseSelectManager(const std::string& request)
{
	this->request = request;
}

const std::string DatabaseSelectManager::processDatabaseNameRequest()
{
    if (request.rfind("GET_DATABASES:", 0) == 0) {
        std::string username = request.substr(14);
        //std::string dbFilePath = "user_databases/" + username + ".txt";
        std::string db_filename = username + ".txt";
        std::ifstream file(db_filename);

        if (!file.is_open()) {
            AppLog log;
            log.write("[SERVER] Could not open database list file: " + db_filename);
            return "DBLIST:";  // trimite rsspuns gol
        }

        if (!file.is_open()) {
            return "DBLIST:";  // Nu exista baze de date
        }

        std::string dbName;
        std::string response = "DBLIST:";
        bool first = true;

        while (std::getline(file, dbName)) {
            if (!first) {
                response += ";";
            }
            response += dbName;
            first = false;
        }
        return response;
    }
}
