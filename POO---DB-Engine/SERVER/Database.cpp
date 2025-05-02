#include "Database.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

Database::Database(const std::string& dbName) : name(dbName) {}

const std::string& Database::getName() const {
    return name;
}

void Database::addTable(const Table& table) {
    tables[table.getName()] = table;
}

bool Database::hasTable(const std::string& tableName) const {
    return tables.find(tableName) != tables.end();
}

Table* Database::getTable(const std::string& tableName) {
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return &(it->second);
    }
    return nullptr;
}

void Database::dropTable(const std::string& tableName) {
    tables.erase(tableName);
}

std::vector<std::string> Database::getTableNames() const {
   
    std::vector<std::string> names;
    for (const auto& pair : tables) {
        names.push_back(pair.first);
    }
    return names;
}

void Database::insertRow(const std::string& tableName, const std::vector<std::string>& row)
{
    Table* t = getTable(tableName);
    if (t == nullptr) return;
    if (row.size() != t->getColumns().size()) return; // optional check
    t->addRow(row);
}

void Database::deleteRow(const std::string& tableName, const std::string& pkValue)
{
    Table* t = getTable(tableName);
    if (!t) return;
    t->deleteRowByPK(pkValue);
}

void Database::showRelations()
{
    for (const auto& tablePair : tables) {
        const std::string& tableName = tablePair.first;
        const Table& table = tablePair.second;

        for (const Column& col : table.getColumns()) {
            if (col.isFK()) {
                std::cout << tableName << " -> " << col.getFKTable()
                    << " (" << col.getName() << " -> " << col.getFKColumn() << ")\n";
            }
        }
    }
}

std::string Database::getRelationsAsString() const
{
    std::ostringstream output;
    for (const auto& tablePair : tables) {
        const std::string& tableName = tablePair.first;
        const Table& table = tablePair.second;

        for (const Column& col : table.getColumns()) {
            if (col.isFK()) {
                output << tableName << " -> " << col.getFKTable()
                    << " (" << col.getName() << " -> " << col.getFKColumn() << ")\n";
            }
        }
    }
    return output.str();
}

bool Database::loadFromFile(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        return 0;
    }

    std::string line;
    Table currentTable;
    bool readingColumns = false;
    bool readingRows = false;

    while (std::getline(file, line)) {

        if (line.rfind("TABEL ", 0) == 0) {
            if (!currentTable.getName().empty()) {
                addTable(currentTable);
            }
            currentTable = Table(line.substr(6));
            readingColumns = false;
            readingRows = false;
        }
        else if (line == "#COLUMNS") {
            readingColumns = true;
            readingRows = false;
        }
        else if (line == "#ROWS") {
            readingColumns = false;
            readingRows = true;
        }
        else if (readingColumns) {
            std::istringstream iss(line);
            std::string colName, colType, flag, fkTable, fkColumn;
            iss >> colName >> colType;
            Column column(colName, colType);

            if (iss >> flag) {
                if (flag == "PK") {
                    column.setPrimaryKey();
                }
                else if (flag == "FK") {
                    iss >> fkTable >> fkColumn;
                    column.setForeignKey(fkTable, fkColumn);
                }
            }
            currentTable.addColumn(column);
        }
        else if (readingRows) {
            std::vector<std::string> values;
            std::stringstream ss(line);
            std::string token;
            while (std::getline(ss, token, '|')) {
                values.push_back(token);
            }
            currentTable.addRow(values);
        }
    }

    if (!currentTable.getName().empty()) {
        addTable(currentTable);
    }

    return 1;
}

void Database::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    for (const auto& pair : tables) {
        const Table& table = pair.second;
        file << "TABEL " << table.getName() << "\n";
        file << "#COLUMNS\n";
        for (const Column& col : table.getColumns()) {
            file << col.getName() << " " << col.getType();
            if (col.isPK()) {
                file << " PK";
            }
            else if (col.isFK()) {
                file << " FK " << col.getFKTable() << " " << col.getFKColumn();
            }
            file << "\n";
        }
        file << "#ROWS\n";
        for (const std::vector<std::string>& row : table.getRows()) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i < row.size() - 1) {
                    file << "|";
                }
            }
            file << "\n";
        }
        file << "\n";
    }
}

