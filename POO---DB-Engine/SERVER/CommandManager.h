#pragma once
#include "Database.h"
#include <string>
#include <vector>

class CommandManager {
private:
    Database* workingDatabase;

public:
    CommandManager(const std::string& fullRequest, Database* database);
    std::string processCommand();

private:
    std::string username;
    std::string databaseName;
    std::string userCode;

    void parseRequest(const std::string& fullRequest);
    void splitCommands(const std::string& input, std::vector<std::string>& commands);
    void trim(std::string& s);

    std::string handleCreateTable();
    std::string handleInsert();
    std::string handleDropTable();
    std::string handleSelect();
    std::string handleDelete();
    std::string handleUnknown();
    std::string formatSelectResult(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows);
};