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
