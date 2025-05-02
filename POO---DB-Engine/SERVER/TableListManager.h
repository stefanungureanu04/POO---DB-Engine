#pragma once
#include <string>

class TableListManager {
private:
    std::string username;
    std::string dbName;

public:
    TableListManager(const std::string& request);
    std::string process();
};
