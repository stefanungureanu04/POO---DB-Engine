#pragma once
#include <string>
#include <vector>

enum class EventType {
    AFTER_INSERT,
    AFTER_DELETE
};

struct Trigger {
    std::string name;
    std::string tableName;
    EventType event;
    std::string condition; // optional
    std::vector<std::string> instructions;
};