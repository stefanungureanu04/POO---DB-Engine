#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Table.h"

class Database {
private:
    std::string name; // numele bazei de date (fără extensie .txt)
    std::unordered_map<std::string, Table> tables;

public:
    Database(const std::string& dbName);

    const std::string& getName() const;
    void addTable(const Table& table);
    bool hasTable(const std::string& tableName) const;
    Table* getTable(const std::string& tableName);
    void dropTable(const std::string& tableName);
    std::vector<std::string> getTableNames() const;
    void insertRow(const std::string& tableName, const std::vector<std::string>& row);
    void deleteRow(const std::string& tableName, const std::string& pkValue);

    void showRelations();
    std::string getRelationsAsString() const;

    bool loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
};
