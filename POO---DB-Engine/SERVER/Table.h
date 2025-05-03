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
    std::vector<std::vector<std::string>> rows; // each row is a vector of strings.

public:
    Table() {}
    Table(const std::string& name, const std::vector<Column>& cols);
    Table(const std::string& name);

    void addColumn(const Column& column);
    void addRow(const std::vector<std::string>& row);
    void deleteRow(size_t row_index);
    void deleteRowByPK(const std::string& pkValue);
    const std::string& getName() const;
    const std::vector<Column>& getColumns() const;
    std::vector<std::vector<std::string>>& getRows();
    const std::vector<std::vector<std::string>>& getRows() const;

    int deleteRowsWhere(const std::string& colName, const std::string& op, const std::string& value);
};

