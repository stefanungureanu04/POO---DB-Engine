#pragma once
#include <string>

class RelationManager {
private:
    std::string username;
    std::string dbName;

public:
    RelationManager(const std::string& request);
    std::string process();
};