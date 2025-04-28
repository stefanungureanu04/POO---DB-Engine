#pragma once
#include <string>

class CommandManager {
public:
    CommandManager(const std::string& fullRequest);

    std::string processCommand();

private:
    std::string username;
    std::string databaseName;
    std::string userCode;

    void parseRequest(const std::string& fullRequest);
    std::string handleCreateTable();
    std::string handleInsert();
    std::string handleSelect();
    std::string handleUnknown();
};