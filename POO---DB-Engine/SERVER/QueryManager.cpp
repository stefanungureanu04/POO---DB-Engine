#include "QueryManager.h"
#include <fstream>
#include <sstream>

QueryManager::QueryManager(const std::string& request)
{
	this->request = request;
}

const std::string QueryManager::processQueryRequest()
{
    if (request.rfind("SAVE_QUERY:", 0) == 0) {
        
        std::string data = request.substr(11);
        size_t sep1 = data.find(':');
        size_t sep2 = data.find(':', sep1 + 1);

        if (sep1 == std::string::npos || sep2 == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep1);
        std::string queryName = data.substr(sep1 + 1, sep2 - sep1 - 1);
        std::string queryContent = data.substr(sep2 + 1);

        std::string queryFilename = queryName + "_qry.txt";
        std::string userQueriesFile = username + "_queries.txt";

        // Save query content
        std::ofstream queryFile(queryFilename);
        if (!queryFile.is_open()) return "SAVE_FAIL";
        queryFile << queryContent;
        queryFile.close();

        // Check if query name already exists in userQueriesFile
        bool alreadyExists = false;
        std::ifstream inFile(userQueriesFile);
        std::string line;
        while (std::getline(inFile, line)) {
            if (line == queryName) {
                alreadyExists = true;
                break;
            }
        }
        inFile.close();

        // If not already listed, append query name to username_queries.txt
        if (!alreadyExists) {
            std::ofstream outFile(userQueriesFile, std::ios::app);
            if (!outFile.is_open()) return "SAVE_FAIL";
            outFile << queryName << "\n";
            outFile.close();
        }

        return "SAVE_SUCCESS";
    }


    else if (request.rfind("LIST_QUERIES:", 0) == 0) {
        std::string username = request.substr(13);
        std::string userQueriesFile = username + "_queries.txt";

        std::ifstream inFile(userQueriesFile);
        if (!inFile.is_open()) return "QUERYLIST:"; // empty list if file missing

        std::string response = "QUERYLIST:";
        std::string queryName;
        bool first = true;
        while (std::getline(inFile, queryName)) {
            if (!first) response += "|||";
            response += queryName;
            first = false;
        }
        inFile.close();

        return response;
    }

    else if (request.rfind("LOAD_QUERY:", 0) == 0) {
        std::string data = request.substr(11);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string queryName = data.substr(sep + 1);

        std::string queryFilename = queryName + "_qry.txt";

        std::ifstream inFile(queryFilename);
        if (!inFile.is_open()) return "LOAD_FAIL";

        std::ostringstream oss;
        oss << inFile.rdbuf();
        return "QUERYDATA:" + oss.str();
    }
}
