#include <vector>
#include <sstream>
#include "StoredProcedure.h"

StoredProcedure::StoredProcedure(const std::string& name, const std::vector<std::string>& stmts) : name(name), sqlStatements(stmts) {}

const std::string& StoredProcedure::getName() const
{
	return name;
}

const std::vector<std::string>& StoredProcedure::getStatements() const
{
	return sqlStatements;
}
void StoredProcedure::adjust()
{
    auto it = sqlStatements.begin();
    while (it != sqlStatements.end()) {
        std::string trimmed = *it;
        trim(trimmed);
        if (trimmed == "end" || trimmed == "end;") {
            it = sqlStatements.erase(it);
        }
        else {
            ++it;
        }
    }
}

std::string StoredProcedure::serialize() const
{
    std::ostringstream oss;
    oss << "#PROCEDURE " << name << "\n";
    for (const auto& stmt : sqlStatements)
        oss << stmt << "\n";
    oss << "end;\n";
    return oss.str();
}

StoredProcedure StoredProcedure::deserialize(std::istream& in)
{
    std::string header;
    std::getline(in, header);
    std::string procName = header.substr(11);  // skip "#PROCEDURE "

    std::vector<std::string> stmts;
    std::string line;
    while (std::getline(in, line)) {
        if (line == "end;" || line == "END;") break;
        stmts.push_back(line);
    }

    return StoredProcedure(procName, stmts);
}
