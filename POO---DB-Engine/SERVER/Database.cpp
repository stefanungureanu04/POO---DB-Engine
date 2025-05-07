#include "Database.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "CommandManager.h"


Database::Database(const std::string& dbName) : name(dbName) {}

const std::string& Database::getName() const 
{
    return name;
}

void Database::addTable(const Table& table) 
{
    tables[table.getName()] = table;
}

bool Database::hasTable(const std::string& tableName) const 
{
    return tables.find(tableName) != tables.end();
}

Table* Database::getTable(const std::string& tableName) 
{
    auto it = tables.find(tableName);
    
    if (it != tables.end()) {
        return &(it->second);
    }

    return nullptr;
}

void Database::dropTable(const std::string& tableName) 
{
    tables.erase(tableName);
}

std::vector<std::string> Database::getTableNames() const 
{
   
    std::vector<std::string> names;

    for (const auto& pair : tables) {
        names.push_back(pair.first);
    }

    return names;
}

void Database::insertRow(const std::string& tableName, const std::vector<std::string>& row)
{
    Table* table = getTable(tableName);

    if (table == nullptr) {
        return;
    }
    if (row.size() != table->getColumns().size()) {
        return;
    }
    
    table->addRow(row);
}

void Database::deleteRow(const std::string& tableName, const std::string& pkValue)
{
    Table* table = getTable(tableName);
    
    if (table == nullptr) {
        return;
    }
    
    table->deleteRowByPK(pkValue);
}

int Database::deleteRowsFromTable(const std::string& tableName, const std::string& colName, const std::string& opFound, const std::string& value)
{
    Table* table = getTable(tableName);
    
    if (table ==  nullptr) {
        throw std::runtime_error("Table not found: " + tableName);
    }

    return table->deleteRowsWhere(colName,opFound,value);
}

void Database::addProcedure(const StoredProcedure& proc)
{
    procedures[proc.getName()] = proc;
}

bool Database::hasProcedure(const std::string& name) const
{
    return procedures.find(name) != procedures.end();
}
void Database::dropProcedure(const std::string& name)
{
    procedures.erase(name);
}

StoredProcedure* Database::getProcedure(const std::string& name)
{
    auto it = procedures.find(name);
    return (it != procedures.end()) ? &(it->second) : nullptr;
}

std::string Database::getProceduresInfo() const
{
    std::ostringstream oss;

    if (procedures.empty()) {
        oss << "No stored procedures.\n";
        return oss.str();
    }

    for (const auto& pair : procedures) {
        const std::string& procName = pair.first;
        const StoredProcedure& proc = pair.second;

        oss << procName << " {\n";

        for (const auto& stmt : proc.getStatements()) {
            oss << "   " << stmt << ";\n";
        }

        oss << "}\n\n";
    }

    return oss.str();
}

std::string Database::getSchemaInfo() const
{
    std::ostringstream oss;

    for (const auto& pair : tables)
    {
        const Table& table = pair.second;
    
        oss << "TABLE: " << table.getName() << "\n";
        
        for (const Column& col : table.getColumns()) {
            oss << "  - " << col.getName() << ": " << col.getType();
            if (col.isPK()) {
                oss << " (primary key)";
            }
            if (col.isFK()) {
                oss << " (foreign key references " << col.getFKTable() << "." << col.getFKColumn() << ")";
            }
            oss << "\n";
        }
        oss << "\n";
    }

    return oss.str();
}

void Database::showRelations()
{
    for (const auto& tablePair : tables) {
    
        const std::string& tableName = tablePair.first;
        const Table& table = tablePair.second;

        for (const Column& col : table.getColumns()) {
            if (col.isFK()) {
                std::cout << tableName << "---> " << col.getFKTable() << " (" << col.getName() << " ---> " << col.getFKColumn() << ")\n";
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
                output << tableName << " -> " << col.getFKTable() << " (" << col.getName() << " -> " << col.getFKColumn() << ")\n";
            }
        }
    }
    return output.str();
}

bool Database::loadFromFile(const std::string& filename) {
    filepath = filename;
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    Table currentTable;
    bool readingColumns = false;
    bool readingRows = false;
    bool readingTriggers = false;
    std::ostringstream triggerContent;

    while (std::getline(file, line)) {
        if (line == "#TRIGGERS") {
            while (std::getline(file, line)) {
                trim(line);
                if (line == "#END_TRIGGERS") break;

                if (line.rfind("#TRIGGER", 0) == 0) {
                    std::string trigName = line.substr(8); // get trigger name after "#TRIGGER "
                    trim(trigName);

                    // Read table name and event int
                    if (!std::getline(file, line)) break;
                    trim(line);
                    std::istringstream tableStream(line);
                    std::string tableName;
                    int eventInt;
                    tableStream >> tableName >> eventInt;
                    EventType eventType = static_cast<EventType>(eventInt);

                    // Read params line
                    std::string paramsLine;
                    if (!std::getline(file, paramsLine)) break;
                    trim(paramsLine);

                    // Read instructions until #END_TRIGGER
                    std::vector<std::string> instructions;
                    while (std::getline(file, line)) {
                        trim(line);
                        if (line == "#END_TRIGGER") break;
                        instructions.push_back(line);
                    }

                    // Create and add trigger
                    Trigger trig(trigName, tableName, eventType, paramsLine, instructions);
                    addTrigger(trig);
                }
            }
        }

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
        else if (line == "#PROCEDURES") {
            while (std::getline(file, line)) {
                trim(line);
                if (line.empty()) continue;

                if (line.rfind("PROCEDURE ", 0) == 0) {
                    std::string procName = line.substr(9);
                    trim(procName);
                    std::vector<std::string> procStatements;

                    while (std::getline(file, line)) {
                        trim(line);
                        if (line.empty()) continue;
                        if (line.rfind("PROCEDURE ", 0) == 0 || line[0] == '#') {
                            file.seekg(-((int)line.length() + 2), std::ios_base::cur);
                            break;
                        }
                        if (line.back() == ';') line.pop_back();
                        procStatements.push_back(line);
                    }

                    addProcedure(StoredProcedure(procName, procStatements));
                }
                else if (line[0] == '#') {
                    file.seekg(-((int)line.length() + 2), std::ios_base::cur);
                    break;
                }
            }
        }

        else if (readingColumns) {
            std::istringstream iss(line);
            std::string colName, colType, flag, fkTable, fkColumn;
            iss >> colName >> colType;
            Column column(colName, colType);
            if (iss >> flag) {
                if (flag == "PK") column.setPrimaryKey();
                else if (flag == "FK") {
                    iss >> fkTable >> fkColumn;
                    column.setForeignKey(fkTable, fkColumn);
                }
            }
            currentTable.addColumn(column);
        }
        else if (readingRows) {
            if (!line.empty() && !std::all_of(line.begin(), line.end(), isspace)) {
                std::stringstream ss(line);
                std::string token;
                std::vector<std::string> values;
                while (std::getline(ss, token, '|')) {
                    values.push_back(token);
                }
                currentTable.addRow(values);
            }
        }
    }
    if (!currentTable.getName().empty()) {
        addTable(currentTable);
    }
    return true;
}


void Database::saveToFile()  
{
    std::ofstream file(filepath);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }

    // Salvare triggerelor
    file << "#TRIGGERS\n";
    file << triggerManager.serialize();
    file << "#END_TRIGGERS\n";

    for (const auto& pair : tables) {  //const auto& (safe read)
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
        for (const std::vector<std::string>& row : table.getRows()) {  //  const getRows() used
           
            if (row.empty()) continue;

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

    file << "#PROCEDURES\n";
    for (const auto& pair : procedures) {
        const StoredProcedure& proc = pair.second;
        file << "PROCEDURE " << proc.getName() << "\n";
        for (const auto& stmt : proc.getStatements()) {
            file << stmt << ";" << "\n";
        }
    }
}

void Database::trim(std::string& s)
{
    s.erase(0, s.find_first_not_of(" \t\n\r"));
    s.erase(s.find_last_not_of(" \t\n\r") + 1);
}



void Database::addTrigger(const Trigger& trigger) {
    triggerManager.addTrigger(trigger);
}

void Database::removeTrigger(const std::string& triggerName) {
    triggerManager.removeTrigger(triggerName);
}

std::vector<Trigger> Database::getTriggersForEvent(const std::string& table, EventType event) {
    return triggerManager.getTriggersForEvent(table, event);
}

void Database::executeTriggers(EventType event, const std::string& tableName, CommandManager& cmdMgr) {
    if (cmdMgr.inTriggerExecution) return;
    cmdMgr.inTriggerExecution = true;


    auto triggers = triggerManager.getTriggersForEvent(tableName, event);
    for (const auto& trig : triggers) {
        for (const auto& instr : trig.instructions) {
            cmdMgr.injectInstruction(instr);
        }
    }
    cmdMgr.inTriggerExecution = false;
}

Database* Database::loadDatabaseForUser(const std::string& username, const std::string& dbName) {
    std::string filepath = "databases/" + username + "/" + dbName + ".txt";
    Database* db = new Database(dbName);
    if (!db->loadFromFile(filepath)) {
        delete db;
        return nullptr;
    }
    return db;
}

std::string Database::getTriggersInfo() const
{
    std::ostringstream oss;
    const auto& triggers = triggerManager.getAllTriggers();

    if (triggers.empty()) {
        oss << "No triggers.\n";
        return oss.str();
    }

    for (const auto& trig : triggers) {
        std::string eventStr = (trig.event == EventType::AFTER_INSERT) ? "after insert" :
            (trig.event == EventType::AFTER_DELETE) ? "after delete" : "unknown";

        oss << trig.name << " {\n";
        oss << "    on " << trig.tableName << " " << eventStr << "\n";
        for (const auto& instr : trig.instructions) {
            oss << "    " << instr << ";\n";
        }
        oss << "}\n\n";
    }

    return oss.str();
}
