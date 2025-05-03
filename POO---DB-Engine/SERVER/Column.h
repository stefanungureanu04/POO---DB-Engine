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
    Column(const std::string& name, const std::string& type);

    void setPrimaryKey();
    void setForeignKey(const std::string& foreignTable, const std::string& foreignColumn);
    std::string getName() const;
    std::string getType() const;
    bool isPK() const;
    bool isFK() const;
    std::string getFKTable() const;
    std::string getFKColumn() const;
};