#pragma once
#include "Database.h"
#include <string>
#include <vector>

using namespace std;

class CommandManager {
private:
    Database* workingDatabase;

public:
    CommandManager(const std::string& fullRequest, Database* database);
    std::string processCommand();

    void injectInstruction(const std::string& instruction) {
        injectedInstructions.push_back(instruction);
    }
    
    std::string handleShowTriggers();
    bool inTriggerExecution = false;

private:
    std::string username;
    std::string databaseName;
    std::string userCode;

    std::vector<std::string> injectedInstructions;

    void parseRequest(const std::string& fullRequest);
    void splitCommands(const std::string& input, std::vector<std::string>& commands);
    void trim(std::string& s);

    std::string handleCreateTable();
    std::string handleInsert();
    std::string handleDropTable();
    std::string handleSelect();
    std::string handleDelete();
    std::string handleUnknown();
    std::string handleJoin();  
    std::string handleUpdate();
    std::string handleCreateProcedure();
    std::string handleCallProcedure();
    std::string handleDropProcedure();
	std::string handleCreateTrigger();
	

    bool parseSelectQuery(const string& query, string& columnsPart, string& tableName, string& whereClause, string& orderByColumn);
    bool getSelectedColumns(const vector<Column>& tableCols, const string& columnsPart, vector<int>& colIndexes,vector<string>& selectedColNames);
    bool filterRows(const Table* table, const vector<int>& colIndexes, const vector<Column>& tableCols, const string& whereClause, vector<vector<string>>& selectedRows);
    bool sortRows(vector<vector<string>>& selectedRows, const vector<string>& selectedColNames, const vector<Column>& tableCols, string orderByColumn);
    std::string formatSelectResult(const vector<string>& headers, const vector<vector<string>>& rows);
};