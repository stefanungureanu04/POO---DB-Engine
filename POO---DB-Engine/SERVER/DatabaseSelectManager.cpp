#include "DatabaseSelectManager.h"
#include "AppLog.h"
#include <fstream>
#include <filesystem>

DatabaseSelectManager::DatabaseSelectManager(const std::string& request)
{
	this->request = request;
}

const std::string DatabaseSelectManager::processDatabaseRequest()
{
    if (request.rfind("GET_DATABASES:", 0) == 0) {
        std::string username = request.substr(14);
        std::string userFolder = "databases/" + username;

        if (!std::filesystem::exists(userFolder)) {
            AppLog log;
            log.write("[SERVER] User folder does not exist: " + userFolder);
            return "DBLIST:";  // empty list
        }

        std::string response = "DBLIST:";
        bool first = true;

        for (const auto& entry : std::filesystem::directory_iterator(userFolder)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                // Optionally strip .txt extension
                if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".txt") {
                    filename = filename.substr(0, filename.size() - 4);
                }

                if (!first) response += ";";
                response += filename;
                first = false;
            }
        }
        return response;
    }

    else if (request.rfind("CREATE_DATABASE:", 0) == 0) {
        std::string data = request.substr(16);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string dbName = data.substr(sep + 1);

        std::filesystem::create_directories("databases");
        std::string userFolder = "databases/" + username;
        std::filesystem::create_directories(userFolder);

        std::string dbFilePath = userFolder + "/" + dbName + ".txt";

        if (std::filesystem::exists(dbFilePath)) {
            return "CREATE_DB_FAIL: database already exists";
        }

        std::ofstream dbFile(dbFilePath);
        if (!dbFile.is_open()) return "CREATE_DB_FAIL";

        dbFile.close();
        return "CREATE_DB_SUCCESS";
    }

    else if (request.rfind("DELETE_DATABASE:", 0) == 0) {
        std::string data = request.substr(16);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string dbName = data.substr(sep + 1);

        std::string dbFilePath = "databases/" + username + "/" + dbName + ".txt";

        if (!std::filesystem::exists(dbFilePath)) {
            return "DELETE_DB_FAIL: database does not exist";
        }

        if (std::remove(dbFilePath.c_str()) != 0) {
            return "DELETE_DB_FAIL: unable to delete file";
        }

        return "DELETE_DB_SUCCESS";
    }

    return "UNKNOWN_COMMAND";
}
