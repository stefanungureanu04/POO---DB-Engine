#pragma once
#include <memory>
#include "CurrentDatabase.h"

class CurrentDatabaseManager {
public:
    static void set(std::unique_ptr<CurrentDatabase> db);
    static CurrentDatabase* get();

private:
    static std::unique_ptr<CurrentDatabase> currentDb;
};