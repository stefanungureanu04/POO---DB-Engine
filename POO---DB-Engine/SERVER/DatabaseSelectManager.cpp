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

        std::string db_filename = "databases/" + username + ".txt";

        std::ifstream file(db_filename);
        if (!file.is_open()) {
            AppLog log;
            log.write("[SERVER] Could not open database list file: " + db_filename);
            return "DBLIST:"; 
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

    else if (request.rfind("CREATE_DATABASE:", 0) == 0) {
        std::string data = request.substr(16);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string dbName = data.substr(sep + 1);

        std::filesystem::create_directories("databases");

        std::string userFolder = "databases/" + username;
        std::filesystem::create_directories(userFolder);

        std::string dbFileName = "databases/" + username + ".txt";

        std::ifstream infile(dbFileName);
        bool isEmpty = infile.peek() == std::ifstream::traits_type::eof();
        infile.close();

        std::ofstream file(dbFileName, std::ios::app);
        if (!file.is_open()) return "CREATE_DB_FAIL";

        if (!isEmpty) {
            file << "\n";
        }
        file << dbName;

        std::string dbFilePath = userFolder + "/" + dbName + ".txt";
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

        std::string dbFileName = "databases/" + username + "/" + dbName + ".txt";
        if (std::remove(dbFileName.c_str()) != 0) {
            return "DELETE_DB_FAIL";
        }

        // Remove dbName from username index file
        std::string indexFile = "databases/" + username + ".txt";
        std::ifstream infile(indexFile);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(infile, line)) {
            if (line != dbName) lines.push_back(line);
        }
        infile.close();

        std::ofstream outfile(indexFile, std::ios::trunc);
        for (const auto& l : lines) {
            outfile << l << "\n";
        }

        return "DELETE_DB_SUCCESS";
    }

    return "UNKNOWN_COMMAND";
}
