#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
class Row
{
public:
    Row() = default;
    const std::vector<std::string>& getValues() const { return values; }
    void setValues(const std::vector<std::string>& newValues) { values = newValues; }

private:
    std::vector<std::string> values;
};

