#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Column.h"
#include "Row.h"
class Table
{
public:
    Table(const std::string& name) : name(name) {}

    void addColumn(const Column& column) {
        columns.push_back(column);
    }

    void addRow(const Row& row) {
        rows.push_back(row);
    }

    bool hasPrimaryKeyConflict(const Row& newRow) const {
        auto pkIt = std::find_if(columns.begin(), columns.end(), [](const Column& col) { return col.getIsPrimaryKey(); });
        if (pkIt == columns.end()) return false;
        int pkIndex = std::distance(columns.begin(), pkIt);
        for (const Row& row : rows) {
            if (row.getValues()[pkIndex] == newRow.getValues()[pkIndex]) return true;
        }
        return false;
    }

    void saveToFile(const std::string& path) const {
        std::ofstream file(path);
        if (!file.is_open()) return;

        for (const auto& col : columns) {
            file << col.getName() << "|" << static_cast<int>(col.getType()) << "|" << col.getIsPrimaryKey() << "|" << col.getIsForeignKey() << "|" << col.getForeignKeyReference() << "\n";
        }
        file << "#DATA" << "\n";
        for (const auto& row : rows) {
            const auto& vals = row.getValues();
            for (size_t i = 0; i < vals.size(); ++i) {
                file << vals[i];
                if (i < vals.size() - 1) file << "|";
            }
            file << "\n";
        }
    }

    void loadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return;

        columns.clear();
        rows.clear();
        std::string line;
        bool readingData = false;
        while (std::getline(file, line)) {
            if (line == "#DATA") {
                readingData = true;
                continue;
            }

            std::stringstream ss(line);
            std::string part;
            if (!readingData) {
                std::vector<std::string> parts;
                while (std::getline(ss, part, '|')) parts.push_back(part);
                Column col(parts[0], static_cast<Column::Type>(std::stoi(parts[1])), parts[2] == "1", parts[3] == "1", parts[4]);
                columns.push_back(col);
            }
            else {
                Row row;
                std::vector<std::string> values;
                while (std::getline(ss, part, '|')) values.push_back(part);
                row.setValues(values);
                rows.push_back(row);
            }
        }
    }

    const std::string& getName() const { return name; }
    const std::vector<Column>& getColumns() const { return columns; }
    const std::vector<Row>& getRows() const { return rows; }

private:
    std::string name;
    std::vector<Column> columns;
    std::vector<Row> rows;
};

