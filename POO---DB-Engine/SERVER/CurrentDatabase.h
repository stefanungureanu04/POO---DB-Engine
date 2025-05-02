#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Table.h"

class CurrentDatabase
{
public:
    CurrentDatabase(const std::string& name, const std::string& owner)
        : dbName(name), owner(owner) {}

    void addTable(const Table& table) {
        tables[table.getName()] = table;
    }

    Table* getTable(const std::string& tableName) {
        if (tables.find(tableName) != tables.end()) {
            return &tables[tableName];
        }
        return nullptr;
    }

    void dropTable(const std::string& tableName) {
        tables.erase(tableName);
        std::string fileName = tableName + "_" + dbName + "_" + owner + ".tbl";
        std::remove(fileName.c_str());
    }

    void saveAllTables() const {
        for (const auto& pair : tables) {
            std::string fileName = pair.first + "_" + dbName + "_" + owner + ".tbl";
            pair.second.saveToFile(fileName);
        }
    }

    void loadTable(const std::string& tableName) {
        std::string fileName = tableName + "_" + dbName + "_" + owner + ".tbl";

        std::cout << "Loading table from file: " << fileName << std::endl;  // DEBUG

        Table table(tableName);
        table.loadFromFile(fileName);
        tables[tableName] = table;
    }
    
    std::vector<std::string> getTableNames() const {
        std::vector<std::string> names;
        for (const auto& pair : tables) {
            names.push_back(pair.first);
        }
        return names;
    }

private:
    std::string dbName;
    std::string owner;
    std::unordered_map<std::string, Table> tables;
};

