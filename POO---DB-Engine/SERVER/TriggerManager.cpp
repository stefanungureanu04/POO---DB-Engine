#include "TriggerManager.h"

const std::vector<Trigger>& TriggerManager::getAllTriggers() const {
    if (cachedTriggers.size() != triggers.size()) {
        cachedTriggers.clear();
        for (const auto& [_, trig] : triggers) {
            cachedTriggers.push_back(trig);
        }
    }
    return cachedTriggers;
}
