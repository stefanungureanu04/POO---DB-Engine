#include "Table.h"

void Table::deleteRow(size_t index)
{
    if (index >= rows.size()) {
        return;
    }
    rows.erase(rows.begin() + index);
}

void Table::deleteRowByPK(const std::string& pkValue)
{
    int pkIndex = -1;
    for (size_t i = 0; i < columns.size(); ++i) {
        if (columns[i].isPK()) {
            pkIndex = i;
            break;
        }
    }
    if (pkIndex == -1) return;

    auto it = std::find_if(rows.begin(), rows.end(), [&](const std::vector<std::string>& row) {
        return row[pkIndex] == pkValue;
        });

    if (it != rows.end()) {
        rows.erase(it);
        return;
    }
}

int Table::deleteRowsWhere(const std::string& colName, const std::string& op, const std::string& value)
{
    // Find column index
    int colIndex = -1;
    const Column* colPtr = nullptr;
    for (size_t i = 0; i < columns.size(); ++i) {
        if (columns[i].getName() == colName) {
            colIndex = static_cast<int>(i);
            colPtr = &columns[i];
            break;
        }
    }
    if (colIndex == -1) {
        throw std::runtime_error("Column not found: " + colName);
    }

    bool isNumeric = (colPtr->getType() == "number");

    int deletedCount = 0;
    auto it = rows.begin();

    while (it != rows.end()) {
        if (colIndex >= it->size()) {
            ++it;
            continue;
        }

        const std::string& cell = it->at(colIndex);

        bool match = false;

        if (isNumeric) {
            double cellVal, condVal;
            std::istringstream(cell) >> cellVal;
            std::istringstream(value) >> condVal;

            if (op == "=") match = (cellVal == condVal);
            else if (op == "<") match = (cellVal < condVal);
            else if (op == "<=") match = (cellVal <= condVal);
            else if (op == ">") match = (cellVal > condVal);
            else if (op == ">=") match = (cellVal >= condVal);
            else if (op == "!=") match = (cellVal != condVal);
            else throw std::runtime_error("Unsupported operator: " + op);
        }
        else {
            if (op == "=") match = (cell == value);
            else if (op == "!=") match = (cell != value);
            else throw std::runtime_error("Operator '" + op + "' not supported for string columns.");
        }

        if (match) {
            it = rows.erase(it);
            deletedCount++;
        }
        else {
            ++it;
        }
    }

    return deletedCount;
}
