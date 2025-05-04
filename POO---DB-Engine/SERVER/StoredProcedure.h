#pragma once
#include <string>

class StoredProcedure {
private:
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> sqlStatements;

private:
    void trim(std::string& s) {
        s.erase(0, s.find_first_not_of(" \t\n\r"));
        s.erase(s.find_last_not_of(" \t\n\r") + 1);
    }

public:
    StoredProcedure() = default;
    StoredProcedure(const std::string& name, const std::vector<std::string>& stmts);
    const std::string& getName() const;
    const std::vector<std::string>& getStatements() const;
    void adjust();
    std::string serialize() const;
    static StoredProcedure deserialize(std::istream& in);
};
