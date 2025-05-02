#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include "Table.h"

class CurrentDatabase
{
public:
    CurrentDatabase(const std::string& name, const std::string& owner)
        : dbName(name), owner(owner) {}

    std::string getName() {
        return dbName;
    }

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
           // pair.second.saveToFile(fileName);
        }
    }

    void loadTable(const std::string& tableName) {
        std::string fileName = tableName + "_" + dbName + "_" + owner + ".tbl";
        Table table(tableName);
        //table.loadFromFile(fileName);
        tables[tableName] = table;
    }
    
    std::vector<std::string> getTableNames() const {
        std::vector<std::string> names;

        // Build the correct folder path
        std::string folderPath = "databases/" + owner + "/" + dbName + "/";

        try {
            for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".tbl") {
                        std::string tableName = filename.substr(0, filename.size() - 4); // remove ".tbl"
                        names.push_back(tableName);
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[getTableNames] Filesystem error: " << e.what() << std::endl;
            // Optionally handle/log or return empty
        }

        return names;
    }

private:
    std::string dbName;
    std::string owner;
    std::unordered_map<std::string, Table> tables;
};

