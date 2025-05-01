#include "LogProviderManager.h"
#include <string>
#include <vector>
#include <fstream>

LogProviderManager::LogProviderManager(const std::string& request)
{
	this->request = request;
}

const std::string LogProviderManager::processLogRequests()
{
    if (request.rfind("GET_LOGS:", 0) == 0) {
        std::string username = request.substr(9);
        std::ifstream file("log.txt");

        if (!file.is_open()) {
            return "LOGDATA:"; 
        }

        std::vector<std::string> logs;
        std::string line;
        while (std::getline(file, line)) {
            if (line.find(username) != std::string::npos) {
                logs.push_back(line);
            }
        }

        std::string response = "LOGDATA:";
        for (size_t i = 0; i < logs.size(); ++i) {
            response += logs[i];
            if (i < logs.size() - 1) {
                response += "|||";
            }
        }

        response += "|||END_OF_LOGS";
        return response;
    }
}
