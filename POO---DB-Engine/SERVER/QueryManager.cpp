#include "QueryManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>

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

        std::string userFolder = "queries/" + username + "/";
        std::filesystem::create_directories(userFolder);  

        std::string queryFilename = userFolder + queryName + "_qry.txt";

        std::ofstream queryFile(queryFilename);
        if (!queryFile.is_open()) return "SAVE_FAIL";
        queryFile << queryContent;
        queryFile.close();

        return "SAVE_SUCCESS";
    }

    else if (request.rfind("LIST_QUERIES:", 0) == 0) {
        std::string username = request.substr(13);

        std::string userFolder = "queries/" + username + "/";
        std::filesystem::create_directories(userFolder);  

        std::string response = "QUERYLIST:";
        bool first = true;

        for (const auto& entry : std::filesystem::directory_iterator(userFolder)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.size() >= 8 && filename.substr(filename.size() - 8) == "_qry.txt") {
                    std::string queryName = filename.substr(0, filename.size() - 8);  
                    if (!first) response += "|||";
                    response += queryName;
                    first = false;
                }
            }
        }

        return response;
    }

    else if (request.rfind("LOAD_QUERY:", 0) == 0) {
        std::string data = request.substr(11);
        size_t sep = data.find(':');
        if (sep == std::string::npos) return "INVALID_FORMAT";

        std::string username = data.substr(0, sep);
        std::string queryName = data.substr(sep + 1);

        std::string userFolder = "queries/" + username + "/";
        std::filesystem::create_directories(userFolder);

        std::string queryFilename = userFolder + queryName + "_qry.txt";

        std::ifstream inFile(queryFilename);
        if (!inFile.is_open()) return "LOAD_FAIL";

        std::ostringstream oss;
        oss << inFile.rdbuf();

        return "QUERYDATA:" + oss.str();
    }

    return "UNKNOWN_COMMAND";
}
