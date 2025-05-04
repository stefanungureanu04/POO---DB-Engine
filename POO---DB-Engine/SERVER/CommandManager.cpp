#include "CommandManager.h"
#include <sstream>
#include <algorithm>
#include <iomanip>

CommandManager::CommandManager(const std::string& fullRequest, Database* database)
{
    workingDatabase = database;

    parseRequest(fullRequest);
}

void CommandManager::parseRequest(const std::string& fullRequest) 
{
    size_t pos1 = fullRequest.find(':');
    size_t pos2 = fullRequest.find(':', pos1 + 1);
    size_t pos3 = fullRequest.find(':', pos2 + 1);

    if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
        return;
    }

    username = fullRequest.substr(pos1 + 1, pos2 - pos1 - 1);
    databaseName = fullRequest.substr(pos2 + 1, pos3 - pos2 - 1);
    userCode = fullRequest.substr(pos3 + 1);
}

void CommandManager::splitCommands(const std::string& input, std::vector<std::string>& commands)
{
    std::istringstream iss(input);
    std::string command;
    while (std::getline(iss, command, ';')) {
        commands.push_back(command);
    }
}

void CommandManager::trim(std::string& s)
{
    s.erase(0, s.find_first_not_of(" \t\n\r"));
    s.erase(s.find_last_not_of(" \t\n\r") + 1);
}

std::string CommandManager::processCommand() 
{
    std::istringstream iss(userCode);
    std::string command;
    std::string combinedResponse;

    while (std::getline(iss, command, ';')) {
        trim(command);
        if (command.empty()) continue;

        std::string lowerCommand = command;
        std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);

        std::string response;

        if (lowerCommand.find("create table") == 0) {
            userCode = command;  // set current command
            response = handleCreateTable();
        }
        else if (lowerCommand.find("insert into") == 0) {
            userCode = command;
            response = handleInsert();
        }
        else if (lowerCommand.find("delete from") == 0) {
            userCode = command;
            response = handleDelete();
        }
        else if (lowerCommand.find("drop table") == 0) {
            userCode = command;
            response = handleDropTable();
        }
        else if (lowerCommand.find("join") != std::string::npos) {
            userCode = command;
            response = handleJoin();
        }
        else if (lowerCommand.find("select") == 0) {
            userCode = command;
            //voi redirectiona fluxul de procesare spre join in cazul in care apare group by
            if (command.find("JOIN") != std::string::npos ||
                command.find("join") != std::string::npos ||
                command.find("GROUP BY") != std::string::npos ||
                command.find("group by") != std::string::npos) {
                response = handleJoin();
            }
            else {
                response = handleSelect();
            }
        }
        else if (lowerCommand.find("update table") == 0) {
            userCode = command;
            response = handleUpdate();
        }
        else if (lowerCommand.find("create procedure") == 0) {
            userCode = command;
            response = handleCreateProcedure();
        }
        else if (lowerCommand.find("call") == 0) {
            userCode = command;
            response = handleCallProcedure();
        }
        else if (lowerCommand.find("drop procedure") == 0) {
            userCode = command;
            response = handleDropProcedure();
        }
        else {
            response = handleUnknown();
        }

        if (!combinedResponse.empty()) {
            combinedResponse += "\n";  // separate responses
        }

        combinedResponse += response;
    }

    return combinedResponse;
}

std::string CommandManager::handleCreateTable() 
{
    size_t nameStart = userCode.find("table") + 5;
    size_t parenStart = userCode.find("(", nameStart);
    size_t parenEnd = userCode.find(")", parenStart);

    if (nameStart == std::string::npos || parenStart == std::string::npos || parenEnd == std::string::npos) {
        return "CREATE_TABLE_FAIL: syntax error";
    }

    std::string tableName = userCode.substr(nameStart, parenStart - nameStart);
    trim(tableName);

    std::string columnsDef = userCode.substr(parenStart + 1, parenEnd - parenStart - 1);

    std::vector<Column> columns;
    std::istringstream iss(columnsDef);
    std::string colDef;
    while (std::getline(iss, colDef, ',')) {
        trim(colDef);

        std::istringstream colStream(colDef);
        std::string colName, colType, flag, fkTable, fkColumn;
        colStream >> colName >> colType;

        Column column(colName, colType);

        if (colStream >> flag) {
            if (flag == "PK") {
                column.setPrimaryKey();
            }
            else if (flag == "FK") {
                colStream >> fkTable >> fkColumn;
                column.setForeignKey(fkTable, fkColumn);
            }
        }
        columns.push_back(column);
    }

    if (workingDatabase->hasTable(tableName)) {
        return "CREATE_TABLE_FAIL: table already exists";
    }

    Table newTable(tableName, columns);
    workingDatabase->addTable(newTable);

    try {
        workingDatabase->saveToFile();
    }
    catch (const std::exception& ex) {
        return "CREATE_TABLE_SUCCESS but failed to save: " + std::string(ex.what());
    }

    return "CREATE_TABLE_SUCCESS";
}

std::string CommandManager::handleInsert() 
{
    size_t intoPos = userCode.find("into") + 4;
    size_t valuesPos = userCode.find("values", intoPos);

    if (intoPos == std::string::npos || valuesPos == std::string::npos) {
        return "INSERT_FAIL: syntax error";
    }

    std::string tableName = userCode.substr(intoPos, valuesPos - intoPos);
    trim(tableName);

    size_t parenStart = userCode.find("(", valuesPos);
    size_t parenEnd = userCode.find(")", parenStart);

    if (parenStart == std::string::npos || parenEnd == std::string::npos) {
        return "INSERT_FAIL: missing parentheses";
    }

    std::string valuesStr = userCode.substr(parenStart + 1, parenEnd - parenStart - 1);
    std::vector<std::string> values;
    std::istringstream iss(valuesStr);
    std::string val;
    while (std::getline(iss, val, ',')) {
        trim(val);
        values.push_back(val);
    }

    Table* table = workingDatabase->getTable(tableName);
    if (!table) {
        return "INSERT_FAIL: table not found";
    }

    if (values.size() != table->getColumns().size()) {
        return "INSERT_FAIL: column count mismatch";
    }

    table->addRow(values);

    try {
        workingDatabase->saveToFile();
    }
    catch (const std::exception& ex) {
        return "INSERT_SUCCESS but failed to save: " + std::string(ex.what());
    }

    return "INSERT_SUCCESS";
}

std::string CommandManager::handleDropTable()
{
    size_t namePos = userCode.find("table") + 5;
    if (namePos == std::string::npos) {
        return "DROP_TABLE_FAIL: syntax error";
    }

    std::string tableName = userCode.substr(namePos);
    trim(tableName);

    if (!workingDatabase->hasTable(tableName)) {
        return "DROP_TABLE_FAIL: table not found";
    }

    workingDatabase->dropTable(tableName);

    try {
        workingDatabase->saveToFile();
    }
    catch (const std::exception& ex) {
        return "DROP_TABLE_SUCCESS but failed to save: " + std::string(ex.what());
    }

    return "DROP_TABLE_SUCCESS";
}

std::string CommandManager::handleSelect() 
{
    std::string columnsPart, tableName, whereClause, orderByColumn;

    if (!parseSelectQuery(userCode, columnsPart, tableName, whereClause, orderByColumn))
        return "SELECT_FAIL: invalid syntax";

    Table* table = workingDatabase->getTable(tableName);
    if (!table)
        return "SELECT_FAIL: table not found";

    const auto& tableCols = table->getColumns();
    std::vector<int> colIndexes;
    std::vector<std::string> selectedColNames;

    if (!getSelectedColumns(tableCols, columnsPart, colIndexes, selectedColNames))
        return "SELECT_FAIL: unknown column";

    std::vector<std::vector<std::string>> selectedRows;
    if (!filterRows(table, colIndexes, tableCols, whereClause, selectedRows))
        return "SELECT_FAIL: error in WHERE clause";

    if (!sortRows(selectedRows, selectedColNames, tableCols, orderByColumn))
        return "SELECT_FAIL: error in ORDER BY";

    return formatSelectResult(selectedColNames, selectedRows);
}

std::string CommandManager::handleDelete()
{
    size_t fromPos = userCode.find("from") + 4;
    size_t wherePos = userCode.find("where", fromPos);

    if (fromPos == std::string::npos || wherePos == std::string::npos) {
        return "DELETE_FAIL: syntax error";
    }

    std::string tableName = userCode.substr(fromPos, wherePos - fromPos);
    trim(tableName);

    std::string condition = userCode.substr(wherePos + 5);
    trim(condition);

    // ✅ Parse operator
    std::string operators[] = { ">=", "<=", "!=", "=", "<", ">" };
    std::string opFound;
    size_t opPos = std::string::npos;

    for (const std::string& op : operators) {
        opPos = condition.find(op);
        if (opPos != std::string::npos) {
            opFound = op;
            break;
        }
    }

    if (opFound.empty()) {
        return "DELETE_FAIL: invalid operator";
    }

    std::string colName = condition.substr(0, opPos);
    std::string value = condition.substr(opPos + opFound.length());
    trim(colName);
    trim(value);

    try {
        int deleted = workingDatabase->deleteRowsFromTable(tableName, colName, opFound, value);
        workingDatabase->saveToFile();

        if (deleted == 0) {
            return "DELETE_SUCCESS: no rows deleted";
        }
        return "DELETE_SUCCESS: " + std::to_string(deleted) + " rows deleted";
    }
    catch (const std::exception& e) {
        return std::string("DELETE_FAIL: ") + e.what();
    }
 }


std::string CommandManager::handleJoin() {
    
    std::string query = userCode;

    size_t selectPos = query.find("select");
    size_t fromPos = query.find("from");
    size_t wherePos = query.find("where", fromPos);
    size_t groupByPos = query.find("group by", fromPos);
    size_t havingPos = query.find("having", fromPos);
    size_t orderByPos = query.find("order by", fromPos);

    if (selectPos == std::string::npos || fromPos == std::string::npos)
        return "JOIN_FAIL: missing SELECT or FROM";

    size_t clauseEnd = std::min({
        wherePos != std::string::npos ? wherePos : query.size(),
        orderByPos != std::string::npos ? orderByPos : query.size(),
        groupByPos != std::string::npos ? groupByPos : query.size(),
        havingPos != std::string::npos ? havingPos : query.size()
        });

    std::string selectPart = query.substr(selectPos + 6, fromPos - (selectPos + 6));
    std::string joinSection = query.substr(fromPos + 4, clauseEnd - (fromPos + 4));
    std::string whereClause = wherePos != std::string::npos ? query.substr(wherePos + 5, (groupByPos != std::string::npos ? groupByPos : query.size()) - (wherePos + 5)) : "";
    std::string groupByCol = groupByPos != std::string::npos ? query.substr(groupByPos + 8, (havingPos != std::string::npos ? havingPos : query.size()) - (groupByPos + 8)) : "";
    std::string havingClause = havingPos != std::string::npos ? query.substr(havingPos + 6, (orderByPos != std::string::npos ? orderByPos : query.size()) - (havingPos + 6)) : "";
    std::string orderByCol = orderByPos != std::string::npos ? query.substr(orderByPos + 8) : "";
    bool orderDesc = false;

    trim(selectPart); trim(joinSection); trim(whereClause);
    trim(groupByCol); trim(havingClause); trim(orderByCol);

    if (orderByCol.size() >= 4 && orderByCol.substr(orderByCol.size() - 4) == "desc") {
        orderDesc = true;
        orderByCol = orderByCol.substr(0, orderByCol.size() - 4);
        trim(orderByCol);
    }

    std::vector<std::string> selectedCols;
    struct Aggregate { std::string function, column, alias; };
    std::vector<Aggregate> aggregates;

    std::istringstream selStream(selectPart);
    std::string token;
    while (std::getline(selStream, token, ',')) {
        trim(token);
        std::string lower = token;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower.find("count(") == 0 || lower.find("sum(") == 0 || lower.find("avg(") == 0 || lower.find("min(") == 0 || lower.find("max(") == 0) {
            size_t open = token.find('('), close = token.find(')');
            std::string func = token.substr(0, open);
            std::string col = token.substr(open + 1, close - open - 1);
            trim(func); trim(col);
            std::string alias = func + "(" + col + ")";
            std::transform(func.begin(), func.end(), func.begin(), ::tolower);
            aggregates.push_back({ func, col, alias });
            selectedCols.push_back(alias);
        }
        else {
            std::transform(token.begin(), token.end(), token.begin(), ::tolower);
            selectedCols.push_back(token);
        }
    }

    struct JoinPart {
        std::string table;
        std::string alias;
        std::string joinLeft;
        std::string joinRight;
        bool isLeftJoin = false;
    };

    std::unordered_map<std::string, std::string> aliasMap;
    std::vector<JoinPart> tables;
    std::istringstream iss(joinSection);
    std::string baseTable, maybeAs, alias;
    iss >> baseTable >> maybeAs;

    std::transform(baseTable.begin(), baseTable.end(), baseTable.begin(), ::tolower);

    if (maybeAs == "AS" || maybeAs == "as") {
        iss >> alias;
    }
    else if (maybeAs == "left" || maybeAs == "join" || maybeAs == "on") {
        alias = baseTable;
        iss.clear();
        iss.seekg(joinSection.find(baseTable) + baseTable.length());
    }
    else {
        alias = maybeAs;
    }
    std::transform(alias.begin(), alias.end(), alias.begin(), ::tolower);

    aliasMap[alias] = baseTable;
    tables.push_back({ baseTable, alias, "", "" });

    std::string word;
    while (iss >> word) {
        bool isLeft = false;
        if (word == "left") {
            iss >> word;
            if (word != "join") return "JOIN_FAIL: expected JOIN after LEFT";
            isLeft = true;
        }
        else if (word != "join") continue;

        std::string newTable, maybeAs2, newAlias, onWord;
        iss >> newTable >> maybeAs2;

        if (maybeAs2 == "AS" || maybeAs2 == "as") {
            iss >> newAlias;
            iss >> onWord;
        }
        else if (maybeAs2 == "ON" || maybeAs2 == "on") {
            onWord = maybeAs2;
            newAlias = newTable;
        }
        else {
            newAlias = maybeAs2;
            iss >> onWord;
        }

        if (onWord != "on" && onWord != "ON") return "JOIN_FAIL: expected ON";

        std::string condition, temp;
        while (iss >> temp) {
            if (temp == "join" || temp == "left" || temp == "where" || temp == "order" || temp == "group") {
                for (int i = temp.length() - 1; i >= 0; --i)
                    iss.putback(temp[i]);
                break;
            }
            condition += temp + " ";
        }
        trim(condition);
        size_t eq = condition.find('=');
        if (eq == std::string::npos) return "JOIN_FAIL: invalid join condition";

        std::string left = condition.substr(0, eq), right = condition.substr(eq + 1);
        trim(left); trim(right);
        std::transform(newTable.begin(), newTable.end(), newTable.begin(), ::tolower);
        std::transform(newAlias.begin(), newAlias.end(), newAlias.begin(), ::tolower);
        std::transform(left.begin(), left.end(), left.begin(), ::tolower);
        std::transform(right.begin(), right.end(), right.begin(), ::tolower);

        aliasMap[newAlias] = newTable;
        tables.push_back({ newTable, newAlias, left, right, isLeft });
    }

    std::unordered_map<std::string, Table*> tableMap;
    for (const auto& part : tables) {
        if (!workingDatabase->hasTable(part.table))
            return "JOIN_FAIL: table not found: " + part.table;
        tableMap[part.alias] = workingDatabase->getTable(part.table);
    }

    using Row = std::unordered_map<std::string, std::string>;
    std::vector<Row> result;
    Table* firstTable = tableMap[tables[0].alias];
    for (const auto& row : firstTable->getRows()) {
        Row r;
        const auto& cols = firstTable->getColumns();
        for (size_t i = 0; i < cols.size(); ++i) {
            std::string key = tables[0].alias + "." + cols[i].getName();
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            r[key] = row[i];
        }
        result.push_back(r);
    }

    for (size_t i = 1; i < tables.size(); ++i) {
        const auto& part = tables[i];
        Table* rightTable = tableMap[part.alias];
        const auto& rightCols = rightTable->getColumns();
        const auto& rightRows = rightTable->getRows();

        std::vector<Row> newResult;
        for (const auto& existingRow : result) {
            auto it = existingRow.find(part.joinLeft);
            std::string joinValue = (it != existingRow.end()) ? it->second : "";
            bool matched = false;

            for (const auto& rightRow : rightRows) {
                for (size_t j = 0; j < rightCols.size(); ++j) {
                    std::string colName = part.alias + "." + rightCols[j].getName();
                    std::transform(colName.begin(), colName.end(), colName.begin(), ::tolower);
                    if (colName == part.joinRight && rightRow[j] == joinValue) {
                        Row combined = existingRow;
                        for (size_t k = 0; k < rightCols.size(); ++k) {
                            std::string key = part.alias + "." + rightCols[k].getName();
                            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                            combined[key] = rightRow[k];
                        }
                        newResult.push_back(combined);
                        matched = true;
                        break;
                    }
                }
            }
            if (!matched && part.isLeftJoin) {
                Row combined = existingRow;
                for (const auto& col : rightCols) {
                    std::string key = part.alias + "." + col.getName();
                    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                    combined[key] = "NULL";
                }
                newResult.push_back(combined);
            }
        }
        result = std::move(newResult);
    }

    if (!whereClause.empty()) {
        static const std::vector<std::string> ops = { ">=", "<=", "!=", "=", "<", ">" };
        std::string opFound;
        size_t opPos = std::string::npos;

        for (const auto& op : ops) {
            opPos = whereClause.find(op);
            if (opPos != std::string::npos) {
                opFound = op;
                break;
            }
        }

        if (opFound.empty()) return "JOIN_FAIL: invalid WHERE condition";

        std::string left = whereClause.substr(0, opPos), right = whereClause.substr(opPos + opFound.length());
        trim(left); trim(right);
        std::transform(left.begin(), left.end(), left.begin(), ::tolower);
        right.erase(remove(right.begin(), right.end(), '\''), right.end());

        std::vector<Row> filtered;
        for (const auto& row : result) {
            if (!row.count(left)) continue;
            std::string val = row.at(left);
            bool include = false;
            try {
                double a = std::stod(val), b = std::stod(right);
                if (opFound == "=") include = a == b;
                else if (opFound == "!=") include = a != b;
                else if (opFound == ">") include = a > b;
                else if (opFound == "<") include = a < b;
                else if (opFound == ">=") include = a >= b;
                else if (opFound == "<=") include = a <= b;
            }
            catch (...) {
                if (opFound == "=") include = val == right;
                else if (opFound == "!=") include = val != right;
            }
            if (include) filtered.push_back(row);
        }
        result = std::move(filtered);
    }

    //sectiunea unde se aplica functiile agregate
    std::vector<std::vector<std::string>> finalRows;
    if (!groupByCol.empty()) {
        std::unordered_map<std::string, std::vector<Row>> groups;
        for (const auto& row : result) {
            std::string key = row.count(groupByCol) ? row.at(groupByCol) : "NULL";
            groups[key].push_back(row);
        }

        for (const auto& [groupKey, rows] : groups) {
            std::vector<std::string> values;
            for (const auto& col : selectedCols) {
                if (col == groupByCol) {
                    values.push_back(groupKey);
                }
                else {
                    auto it = std::find_if(aggregates.begin(), aggregates.end(), [&](const Aggregate& a) { return a.alias == col; });
                    if (it == aggregates.end()) { values.push_back("NULL"); continue; }
                    const auto& a = *it;
                    if (a.function == "count") {
                        if (a.column == "*" || a.column == " *") {
                            values.push_back(std::to_string(rows.size()));
                        }
                        else {
                            int cnt = 0;
                            for (const auto& r : rows) {
                                if (!r.at(a.column).empty() && r.at(a.column) != "NULL")
                                    cnt++;
                            }
                            values.push_back(std::to_string(cnt));
                        }
                    }
                    else if (a.function == "sum" || a.function == "avg") {
                        double total = 0;
                        for (const auto& r : rows) total += std::stod(r.at(a.column));
                        values.push_back(a.function == "sum" ? std::to_string(total) : std::to_string(total / rows.size()));
                    }
                    else if (a.function == "min") {
                        double minVal = std::stod(rows[0].at(a.column));
                        for (const auto& r : rows) minVal = std::min(minVal, std::stod(r.at(a.column)));
                        values.push_back(std::to_string(minVal));
                    }
                    else if (a.function == "max") {
                        double maxVal = std::stod(rows[0].at(a.column));
                        for (const auto& r : rows) maxVal = std::max(maxVal, std::stod(r.at(a.column)));
                        values.push_back(std::to_string(maxVal));
                    }
                }
            }
            finalRows.push_back(values);
        }
    }
    else {
        for (const auto& r : result) {
            std::vector<std::string> row;
            for (const auto& col : selectedCols) {
                std::string key = col;
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                row.push_back(r.count(key) ? r.at(key) : "NULL");
            }
            finalRows.push_back(row);
        }
    }

    return formatSelectResult(selectedCols, finalRows);
}

std::string CommandManager::handleUpdate()
{
    size_t tablePos = userCode.find("table") + 5;
    size_t setPos = userCode.find("set", tablePos);
    size_t wherePos = userCode.find("where", setPos);

    if (tablePos == std::string::npos || setPos == std::string::npos || wherePos == std::string::npos)
        return "UPDATE_FAIL: syntax error";

    std::string tableName = userCode.substr(tablePos, setPos - tablePos);
    trim(tableName);

    std::string setPart = userCode.substr(setPos + 3, wherePos - (setPos + 3));
    trim(setPart);

    std::string wherePart = userCode.substr(wherePos + 5);
    trim(wherePart);

    // Parse SET part
    std::vector<std::pair<std::string, std::string>> updates;
    std::istringstream setStream(setPart);
    std::string assignment;
    while (std::getline(setStream, assignment, ',')) {
        size_t eqPos = assignment.find('=');
        if (eqPos == std::string::npos) return "UPDATE_FAIL: invalid SET syntax";

        std::string col = assignment.substr(0, eqPos);
        std::string val = assignment.substr(eqPos + 1);
        trim(col);
        trim(val);

        updates.emplace_back(col, val);
    }

    // Parse WHERE
    std::string ops[] = { ">=", "<=", "!=", "=", "<", ">" };
    std::string opFound;
    size_t opPos = std::string::npos;
    for (const std::string& op : ops) {
        opPos = wherePart.find(op);
        if (opPos != std::string::npos) {
            opFound = op;
            break;
        }
    }
    if (opFound.empty()) return "UPDATE_FAIL: invalid WHERE clause";

    std::string whereCol = wherePart.substr(0, opPos);
    std::string whereVal = wherePart.substr(opPos + opFound.length());
    trim(whereCol);
    trim(whereVal);

    Table* table = workingDatabase->getTable(tableName);
    if (!table) return "UPDATE_FAIL: table not found";

    int updatedCount = 0;
    const auto& cols = table->getColumns();
    auto& rows = table->getRows();

    int whereIdx = -1;
    for (size_t i = 0; i < cols.size(); ++i) {
        if (cols[i].getName() == whereCol) {
            whereIdx = static_cast<int>(i);
            break;
        }
    }
    if (whereIdx == -1) return "UPDATE_FAIL: WHERE column not found";

    std::vector<int> updateIndexes;
    for (const auto& up : updates) {
        bool found = false;
        for (size_t i = 0; i < cols.size(); ++i) {
            if (cols[i].getName() == up.first) {
                updateIndexes.push_back(static_cast<int>(i));
                found = true;
                break;
            }
        }
        if (!found) return "UPDATE_FAIL: SET column not found: " + up.first;
    }

    for (auto& row : rows) {
        bool match = false;
        const std::string& cell = row[whereIdx];
        const std::string& colType = cols[whereIdx].getType();

        try {
            if (colType == "number") {
                double a = std::stod(cell), b = std::stod(whereVal);
                if (opFound == "=") match = a == b;
                else if (opFound == "!=") match = a != b;
                else if (opFound == "<") match = a < b;
                else if (opFound == "<=") match = a <= b;
                else if (opFound == ">") match = a > b;
                else if (opFound == ">=") match = a >= b;
            }
            else {
                if (opFound == "=") match = cell == whereVal;
                else if (opFound == "!=") match = cell != whereVal;
                else return "UPDATE_FAIL: unsupported string operator: " + opFound;
            }
        }
        catch (...) { return "UPDATE_FAIL: invalid WHERE value"; }

        if (match) {
            for (size_t i = 0; i < updates.size(); ++i) {
                row[updateIndexes[i]] = updates[i].second;
            }
            updatedCount++;
        }
    }

    try {
        workingDatabase->saveToFile();
    }
    catch (const std::exception& ex) {
        return "UPDATE_SUCCESS: but failed to save: " + std::string(ex.what());
    }

    return "UPDATE_SUCCESS: " + std::to_string(updatedCount) + " rows updated";
}

std::string CommandManager::handleCreateProcedure()
{
    std::istringstream iss(userCode);
    std::string line;

    // Read procedure name from the first line
    std::getline(iss, line);
    trim(line);

    if (line.rfind("create procedure", 0) != 0) {
        return "CREATE_PROCEDURE_FAIL: missing 'create procedure'";
    }

    std::string procName = line.substr(16); // after "create procedure"
    trim(procName);

    if (procName.empty()) {
        return "CREATE_PROCEDURE_FAIL: missing procedure name";
    }

    std::vector<std::string> statements;

    // Read following lines
    while (std::getline(iss, line)) {
        trim(line);

        if (line == "end;" || line == "END;") {
            break;  // ✅ End of procedure
        }

        if (line.find("end;") != std::string::npos && line != "end;") {
            return "CREATE_PROCEDURE_FAIL: 'end;' must be on its own line";
        }

        if (line.rfind("instruction ", 0) != 0) {
            return "CREATE_PROCEDURE_FAIL: expected 'instruction' at start of line";
        }

        std::string stmt = line.substr(11); // remove "instruction "
        trim(stmt);

        if (stmt.empty()) {
            return "CREATE_PROCEDURE_FAIL: empty instruction";
        }

        statements.push_back(stmt);
    }

    if (statements.empty()) {
        return "CREATE_PROCEDURE_FAIL: no instructions";
    }

    StoredProcedure proc(procName, statements);
    proc.adjust();
    workingDatabase->addProcedure(proc);

    try {
        workingDatabase->saveToFile();
    }
    catch (const std::exception& ex) {
        return "CREATE_PROCEDURE_SUCCESS but failed to save: " + std::string(ex.what());
    }

    return "CREATE_PROCEDURE_SUCCESS";
}

std::string CommandManager::handleCallProcedure()
{
    std::string callLine = userCode;
    trim(callLine);

    if (callLine.rfind("call ", 0) != 0) {
        return "CALL_FAIL: invalid call syntax";
    }

    callLine = callLine.substr(5);  // remove "call "
    trim(callLine);

    // optional semicolon
    if (!callLine.empty() && callLine.back() == ';')
        callLine.pop_back();
    trim(callLine);

    // split procName and argument list
    size_t parenStart = callLine.find('(');
    size_t parenEnd = callLine.find(')');

    if (parenStart == std::string::npos || parenEnd == std::string::npos || parenEnd < parenStart) {
        return "CALL_FAIL: invalid parentheses";
    }

    std::string procName = callLine.substr(0, parenStart);
    trim(procName);

    std::string argList = callLine.substr(parenStart + 1, parenEnd - parenStart - 1);
    std::vector<std::string> arguments;
    std::istringstream argStream(argList);
    std::string arg;
    while (std::getline(argStream, arg, ',')) {
        trim(arg);
        arguments.push_back(arg);
    }

    // 2️⃣ check if procedure exists
    if (!workingDatabase->hasProcedure(procName)) {
        return "CALL_FAIL: procedure not found";
    }

    StoredProcedure* proc = workingDatabase->getProcedure(procName);
    if (!proc) {
        return "CALL_FAIL: procedure retrieval error";
    }

    // 3️⃣ detect variables in stored statements (IN ORDER of appearance!)
    std::vector<std::string> variables;
    for (const auto& stmt : proc->getStatements()) {
        size_t pos = 0;
        while ((pos = stmt.find("@", pos)) != std::string::npos) {
            size_t end = pos + 1;
            while (end < stmt.size() && (isalnum(stmt[end]) || stmt[end] == '_'))
                ++end;
            std::string varName = stmt.substr(pos + 1, end - pos - 1);
            if (!varName.empty() && std::find(variables.begin(), variables.end(), varName) == variables.end()) {
                variables.push_back(varName);
            }
            pos = end;
        }
    }

    // 4️⃣ map arguments to variables
    if (arguments.size() != variables.size()) {
        return "CALL_FAIL: expected " + std::to_string(variables.size()) + " arguments, got " + std::to_string(arguments.size());
    }

    std::unordered_map<std::string, std::string> paramMap;
    for (size_t i = 0; i < variables.size(); ++i) {
        paramMap["@" + variables[i]] = arguments[i];
    }

    // 5️⃣ perform replacements
    std::vector<std::string> replacedStatements;
    for (auto stmt : proc->getStatements()) {
        for (const auto& [param, value] : paramMap) {
            size_t pos;
            while ((pos = stmt.find(param)) != std::string::npos) {
                stmt.replace(pos, param.length(), value);
            }
        }
        replacedStatements.push_back(stmt);
    }

    // 6️⃣ build combined userCode
    userCode.clear();
    for (const auto& stmt : replacedStatements) {
        userCode += stmt + "; ";
    }

    // 7️⃣ recursively process
    return this->processCommand();
}

std::string CommandManager::handleDropProcedure()
{
    std::string procLine = userCode;
    trim(procLine);

    if (procLine.rfind("drop procedure", 0) != 0) {
        return "DROP_PROCEDURE_FAIL: invalid syntax";
    }

    std::string procName = procLine.substr(14); // after "drop procedure"
    trim(procName);

    if (!procName.empty() && procName.back() == ';')
        procName.pop_back();
    trim(procName);

    if (!workingDatabase->hasProcedure(procName)) {
        return "DROP_PROCEDURE_FAIL: procedure not found";
    }

    workingDatabase->dropProcedure(procName);

    try {
        workingDatabase->saveToFile();
    }
    catch (const std::exception& ex) {
        return "DROP_PROCEDURE_SUCCESS but failed to save: " + std::string(ex.what());
    }

    return "DROP_PROCEDURE_SUCCESS";
}


std::string CommandManager::handleUnknown() 
{
    return "UNKNOWN_COMMAND";
}

bool CommandManager::parseSelectQuery(const std::string& query,
    std::string& columnsPart,
    std::string& tableName,
    std::string& whereClause,
    std::string& orderByColumn)
{
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    size_t selectPos = lowerQuery.find("select");
    size_t fromPos = lowerQuery.find("from");
    if (fromPos == std::string::npos || selectPos == std::string::npos) return false;

    columnsPart = query.substr(selectPos + 6, fromPos - (selectPos + 6));
    std::string afterFrom = query.substr(fromPos + 4);

    trim(columnsPart);
    trim(afterFrom);

    std::string lowerAfterFrom = afterFrom;
    std::transform(lowerAfterFrom.begin(), lowerAfterFrom.end(), lowerAfterFrom.begin(), ::tolower);

    size_t wherePos = lowerAfterFrom.find("where");
    size_t orderByPos = lowerAfterFrom.find("order by");

    if (wherePos != std::string::npos && orderByPos != std::string::npos && orderByPos > wherePos) {
        tableName = afterFrom.substr(0, wherePos);
        whereClause = afterFrom.substr(wherePos + 5, orderByPos - (wherePos + 5));
        orderByColumn = afterFrom.substr(orderByPos + 8);
    }
    else if (wherePos != std::string::npos) {
        tableName = afterFrom.substr(0, wherePos);
        whereClause = afterFrom.substr(wherePos + 5);
    }
    else if (orderByPos != std::string::npos) {
        tableName = afterFrom.substr(0, orderByPos);
        orderByColumn = afterFrom.substr(orderByPos + 8);
    }
    else {
        tableName = afterFrom;
    }

    trim(tableName);
    trim(whereClause);
    trim(orderByColumn);

    return true;
}

bool CommandManager::getSelectedColumns(const std::vector<Column>& tableCols, const std::string& columnsPart, std::vector<int>& colIndexes, std::vector<std::string>& selectedColNames)
{
    if (columnsPart == "*") {
        for (size_t i = 0; i < tableCols.size(); ++i) {
            colIndexes.push_back(static_cast<int>(i));
            selectedColNames.push_back(tableCols[i].getName());
        }
        return true;
    }

    std::istringstream iss(columnsPart);
    std::string colExpr;
    while (std::getline(iss, colExpr, ',')) {
        trim(colExpr);

        std::string baseColName = colExpr;
        std::string alias = "";

        size_t asPos = colExpr.find(" as ");
        if (asPos != std::string::npos) {
            baseColName = colExpr.substr(0, asPos);
            alias = colExpr.substr(asPos + 4);
            trim(baseColName);
            trim(alias);
        }

        bool found = false;
        for (size_t i = 0; i < tableCols.size(); ++i) {
            if (tableCols[i].getName() == baseColName) {
                colIndexes.push_back(static_cast<int>(i));
                selectedColNames.push_back(!alias.empty() ? alias : baseColName);
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

bool CommandManager::filterRows(const Table* table, const std::vector<int>& colIndexes, const std::vector<Column>& tableCols, const std::string& whereClause, std::vector<std::vector<std::string>>& selectedRows)
{
    std::string whereCol, whereOp, whereValue;

    if (!whereClause.empty()) {
        std::string ops[] = { ">=", "<=", "!=", "=", "<", ">" };
        size_t opPos = std::string::npos;

        for (const std::string& op : ops) {
            opPos = whereClause.find(op);
            if (opPos != std::string::npos) {
                whereOp = op;
                break;
            }
        }

        if (whereOp.empty()) return false;

        whereCol = whereClause.substr(0, opPos);
        whereValue = whereClause.substr(opPos + whereOp.length());
        trim(whereCol);
        trim(whereValue);
    }

    for (const auto& row : table->getRows()) {
        bool include = true;

        if (!whereClause.empty()) {
            int whereIdx = -1;
            for (size_t i = 0; i < tableCols.size(); ++i) {
                if (tableCols[i].getName() == whereCol) {
                    whereIdx = static_cast<int>(i);
                    break;
                }
            }
            if (whereIdx == -1) return false;

            const std::string& cell = row[whereIdx];
            const std::string& colType = tableCols[whereIdx].getType();

            if (colType == "number") {
                double cellVal = 0, condVal = 0;
                std::istringstream(cell) >> cellVal;
                std::istringstream(whereValue) >> condVal;

                if (whereOp == "=") include = (cellVal == condVal);
                else if (whereOp == "<") include = (cellVal < condVal);
                else if (whereOp == "<=") include = (cellVal <= condVal);
                else if (whereOp == ">") include = (cellVal > condVal);
                else if (whereOp == ">=") include = (cellVal >= condVal);
                else if (whereOp == "!=") include = (cellVal != condVal);
            }
            else if (colType == "string") {
                if (whereOp == "=") include = (cell == whereValue);
                else if (whereOp == "!=") include = (cell != whereValue);
                else return false;
            }
        }

        if (include) {
            std::vector<std::string> selectedRow;
            for (int colIdx : colIndexes) {
                selectedRow.push_back(row[colIdx]);
            }
            selectedRows.push_back(selectedRow);
        }
    }

    return true;
}

bool CommandManager::sortRows(std::vector<std::vector<std::string>>& selectedRows, const std::vector<std::string>& selectedColNames, const std::vector<Column>& tableCols, std::string orderByColumn)
{
    if (orderByColumn.empty()) return true;

    bool descending = false;
    if (orderByColumn.size() >= 4 && orderByColumn.substr(orderByColumn.size() - 4) == "desc") {
        descending = true;
        orderByColumn = orderByColumn.substr(0, orderByColumn.size() - 4);
        trim(orderByColumn);
    }

    int orderIdx = -1;
    for (size_t i = 0; i < selectedColNames.size(); ++i) {
        if (selectedColNames[i] == orderByColumn) {
            orderIdx = static_cast<int>(i);
            break;
        }
    }
    if (orderIdx == -1) return false;

    int tableOrderIdx = -1;
    for (size_t i = 0; i < tableCols.size(); ++i) {
        if (tableCols[i].getName() == orderByColumn) {
            tableOrderIdx = static_cast<int>(i);
            break;
        }
    }
    if (tableOrderIdx == -1) return false;

    std::string orderByType = tableCols[tableOrderIdx].getType();

    if (orderByType == "number") {
        if (descending) {
            std::sort(selectedRows.begin(), selectedRows.end(),
                [orderIdx](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                    double aVal = 0, bVal = 0;
                    std::istringstream(a[orderIdx]) >> aVal;
                    std::istringstream(b[orderIdx]) >> bVal;
                    return aVal > bVal;
                });
        }
        else {
            std::sort(selectedRows.begin(), selectedRows.end(),
                [orderIdx](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                    double aVal = 0, bVal = 0;
                    std::istringstream(a[orderIdx]) >> aVal;
                    std::istringstream(b[orderIdx]) >> bVal;
                    return aVal < bVal;
                });
        }
    }
    else { // string
        if (descending) {
            std::sort(selectedRows.begin(), selectedRows.end(),
                [orderIdx](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                    return a[orderIdx] > b[orderIdx];
                });
        }
        else {
            std::sort(selectedRows.begin(), selectedRows.end(),
                [orderIdx](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                    return a[orderIdx] < b[orderIdx];
                });
        }
    }

    return true;
}

std::string CommandManager::formatSelectResult(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows)
{
    std::ostringstream oss;
    std::vector<size_t> colWidths(headers.size(), 0);

    // calculate max width per column
    for (size_t i = 0; i < headers.size(); ++i) {
        colWidths[i] = headers[i].length();
    }

    for (const auto& row : rows) {
        for (size_t i = 0; i < headers.size(); ++i) {
            if (i < row.size()) {
                colWidths[i] = std::max(colWidths[i], row[i].length());
            }
        }
    }

    // calculate CRT width
    size_t crtWidth = std::max((size_t)1, std::to_string(rows.size()).length());  // at least width for "CRT"

    // header row
    oss << std::setw(crtWidth) << std::left << "#" << "  ";
    for (size_t i = 0; i < headers.size(); ++i) {
        oss << std::setw(colWidths[i]) << std::left << headers[i] << "  ";
    }
    oss << "\n";

    // underline
    oss << std::string(crtWidth, '-') << "  ";
    for (size_t i = 0; i < headers.size(); ++i) {
        oss << std::string(colWidths[i], '-') << "  ";
    }
    oss << "\n";

    // data rows
    for (size_t rowIdx = 0; rowIdx < rows.size(); ++rowIdx) {
        oss << std::setw(crtWidth) << std::left << (rowIdx + 1) << "  ";
        for (size_t i = 0; i < headers.size(); ++i) {
            if (i < rows[rowIdx].size()) {
                oss << std::setw(colWidths[i]) << std::left << rows[rowIdx][i] << "  ";
            }
            else {
                oss << std::setw(colWidths[i]) << std::left << "NULL" << "  ";
            }
        }
        oss << "\n";
    }

    return oss.str();
}
