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

//aici se proceseaza comanda primita de la client
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
        else if (lowerCommand.find("select") == 0) {
            userCode = command;
            response = handleSelect();
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
    size_t fromPos = userCode.find("from");
    if (fromPos == std::string::npos) {
        return "SELECT_FAIL: missing FROM";
    }

    std::string columnsPart = userCode.substr(7, fromPos - 7); // skip "SELECT "
    std::string afterFrom = userCode.substr(fromPos + 4);      // skip "from"

    trim(columnsPart);
    trim(afterFrom);

    // 2. Parse WHERE and ORDER BY
    size_t wherePos = afterFrom.find("where");
    size_t orderByPos = afterFrom.find("order by");

    std::string tableName;
    std::string whereClause;
    std::string orderByColumn;

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

    // 3. Get table
    Table* table = workingDatabase->getTable(tableName);
    if (!table) {
        return "SELECT_FAIL: table not found";
    }

    const auto& tableCols = table->getColumns();

    // 4. Determine selected columns
    std::vector<int> colIndexes;
    std::vector<std::string> selectedColNames;

    if (columnsPart == "*") {
        for (size_t i = 0; i < tableCols.size(); ++i) {
            colIndexes.push_back(static_cast<int>(i));
            selectedColNames.push_back(tableCols[i].getName());
        }
    }
    else {
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
                    if (!alias.empty()) {
                        selectedColNames.push_back(alias);
                    }
                    else {
                        selectedColNames.push_back(baseColName);
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                return "SELECT_FAIL: column " + baseColName + " not found";
            }
        }
    }

    // 5. Parse WHERE condition
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

        if (whereOp.empty()) {
            return "SELECT_FAIL: invalid WHERE operator";
        }

        whereCol = whereClause.substr(0, opPos);
        whereValue = whereClause.substr(opPos + whereOp.length());

        trim(whereCol);
        trim(whereValue);
    }

    // 6. Gather rows
    std::vector<std::vector<std::string>> selectedRows;

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
            if (whereIdx == -1) {
                return "SELECT_FAIL: column " + whereCol + " not found";
            }

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
                else return "SELECT_FAIL: unsupported operator for string: " + whereOp;
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

    // 7. ORDER BY
    if (!orderByColumn.empty()) {

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
        if (orderIdx == -1) return "SELECT_FAIL: ORDER BY column " + orderByColumn + " not found";

        int tableOrderIdx = -1;
        for (size_t i = 0; i < tableCols.size(); ++i) {
            if (tableCols[i].getName() == orderByColumn) {
                tableOrderIdx = static_cast<int>(i);
                break;
            }
        }
        if (tableOrderIdx == -1) {
            return "SELECT_FAIL: ORDER BY column " + orderByColumn + " not found in table";
        }


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
        else { // string column
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
    }

    // 8. Format
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

std::string CommandManager::handleUnknown() 
{
    return "UNKNOWN_COMMAND";
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
