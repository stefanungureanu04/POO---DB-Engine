#include "CurrentDatabaseManager.h"

std::unique_ptr<CurrentDatabase> CurrentDatabaseManager::currentDb = nullptr;

void CurrentDatabaseManager::set(std::unique_ptr<CurrentDatabase> db) {
    currentDb = std::move(db);
}

CurrentDatabase* CurrentDatabaseManager::get() {
    return currentDb.get();
}