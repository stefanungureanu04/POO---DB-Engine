#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Table.h"
#include "StoredProcedure.h"
#include"TriggerManager.h"
#include <fstream>
#include <sstream>

class Database {
private:
    std::string name; 
    std::unordered_map<std::string, Table> tables;
    std::string filepath;

    std::unordered_map<std::string, StoredProcedure> procedures;

    //triggere
    TriggerManager triggerManager;

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
    int deleteRowsFromTable(const std::string& tableName, const std::string& colName, const std::string& opFound, const std::string& value);

    // PROCEDURES HANDLER
    void addProcedure(const StoredProcedure& proc);
    bool hasProcedure(const std::string& name) const;
    void dropProcedure(const std::string& name);
    StoredProcedure* getProcedure(const std::string& name);
    std::string getProceduresInfo() const;

    std::string getSchemaInfo() const;
    void showRelations();
    std::string getRelationsAsString() const;

    //triggere
    void addTrigger(const Trigger& trigger);
    void removeTrigger(const std::string& triggerName);
    std::vector<Trigger> getTriggersForEvent(const std::string& table, EventType event);

    void executeTriggers(EventType event, const std::string& tableName, class CommandManager& cmdMgr);
    const TriggerManager& getTriggerManager() const { return triggerManager; }
    static Database* loadDatabaseForUser(const std::string& username, const std::string& dbName);
    std::string getTriggersInfo() const;

    bool loadFromFile(const std::string& filename);
    void saveToFile();

private:
    void trim(std::string& s);
};
