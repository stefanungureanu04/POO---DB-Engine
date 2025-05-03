#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Column.h"

class Table {
private:
    std::string name;
    std::vector<Column> columns;
    std::vector<std::vector<std::string>> rows; // fiecare rând e un vector de string-uri

public:
    Table() {}
    Table(const std::string& name, const std::vector<Column>& cols) : name(name), columns(cols) {};
    Table(const std::string& name) : name(name) {}

    void addColumn(const Column& column) { columns.push_back(column); }
    void addRow(const std::vector<std::string>& row) { rows.push_back(row); }
    void deleteRow(size_t row_index);
    void deleteRowByPK(const std::string& pkValue);
    const std::string& getName() const { return name; }
    const std::vector<Column>& getColumns() const { return columns; }
    std::vector<std::vector<std::string>>& getRows() {return rows;}; 
    const std::vector<std::vector<std::string>>& getRows() const { return rows; }

    int deleteRowsWhere(const std::string& colName, const std::string& op, const std::string& value);
};

