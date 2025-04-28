#include "CommandManager.h"
#include <sstream>
#include <algorithm>

CommandManager::CommandManager(const std::string& fullRequest) {
    parseRequest(fullRequest);
}

void CommandManager::parseRequest(const std::string& fullRequest) {
    size_t pos1 = fullRequest.find(':');
    size_t pos2 = fullRequest.find(':', pos1 + 1);
    size_t pos3 = fullRequest.find(':', pos2 + 1);

    if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
        return;
    }

    username = fullRequest.substr(pos1 + 1, pos2 - pos1 - 1);
    databaseName = fullRequest.substr(pos2 + 1, pos3 - pos2 - 1);
    userCode = fullRequest.substr(pos3 + 1);
}

//aici se proceseaza comanda primita de la client
std::string CommandManager::processCommand() {
    std::string lowerCode = userCode;
    std::transform(lowerCode.begin(), lowerCode.end(), lowerCode.begin(), ::tolower);

    if (lowerCode.find("create table") == 0) {
        return handleCreateTable();
    }
    else if (lowerCode.find("insert into") == 0) {
        return handleInsert();
    }
    else if (lowerCode.find("select") == 0) {
        return handleSelect();
    }
    else {
        return handleUnknown();
    }
}

std::string CommandManager::handleCreateTable() {
    // TODO: Creare tabel
    return "CREATE_TABLE_SUCCESS";
}

std::string CommandManager::handleInsert() {
    // TODO: Inserare în tabel
    return "INSERT_SUCCESS";
}

std::string CommandManager::handleSelect() {
    // TODO: Selectare date din tabel
    return "SELECT_SUCCESS";
}

std::string CommandManager::handleUnknown() {
    return "UNKNOWN_COMMAND";
}