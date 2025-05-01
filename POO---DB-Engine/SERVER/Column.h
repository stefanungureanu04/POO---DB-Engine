#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

class Column
{
public:
    enum class Type {
        INT,
        STRING,
        FLOAT,
        DOUBLE,
        BOOL,
        DATE,
        TIME,
        DATETIME,
        CHAR,
        TEXT,
        SMALLINT,
        BIGINT,
        DECIMAL,
        NUMERIC,
        REAL,
        MONEY
    };

    Column(const std::string& name, Type type, bool isPrimaryKey = false, bool isForeignKey = false, const std::string& fkRef = "")
        : name(name), type(type), isPrimaryKey(isPrimaryKey), isForeignKey(isForeignKey), foreignKeyReference(fkRef) {}

    std::string getName() const { return name; }
    Type getType() const { return type; }
    bool getIsPrimaryKey() const { return isPrimaryKey; }
    bool getIsForeignKey() const { return isForeignKey; }
    std::string getForeignKeyReference() const { return foreignKeyReference; }

private:
    std::string name;
    Type type;
    bool isPrimaryKey;
    bool isForeignKey;
    std::string foreignKeyReference;
};

