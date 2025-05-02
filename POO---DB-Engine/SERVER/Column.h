#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

class Column {
private:
    std::string name;
    std::string type;  // "string" or "number"
    bool isPrimaryKey = false;
    bool isForeignKey = false;
    std::string foreignTable;
    std::string foreignColumn;

public:
    Column(const std::string& name, const std::string& type)
        : name(name), type(type) {
    }

    void setPrimaryKey() { isPrimaryKey = true; }
    void setForeignKey(const std::string& foreignTable, const std::string& foreignColumn) {
        isForeignKey = true;
        this->foreignTable = foreignTable;
        this->foreignColumn = foreignColumn;
    }

    std::string getName() const { return name; }
    std::string getType() const { return type; }
    bool isPK() const { return isPrimaryKey; }
    bool isFK() const { return isForeignKey; }
    std::string getFKTable() const { return foreignTable; }
    std::string getFKColumn() const { return foreignColumn; }
};