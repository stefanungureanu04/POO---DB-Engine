#pragma once
#include "Trigger.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

class TriggerManager {
public:
    void addTrigger(const Trigger& trigger) {
        triggers[trigger.name] = trigger;
    }

    void removeTrigger(const std::string& triggerName) {
        triggers.erase(triggerName);
    }

    std::vector<Trigger> getTriggersForEvent(const std::string& table, EventType event) {
        std::vector<Trigger> result;
        for (const auto& [name, trigger] : triggers) {
            if (trigger.tableName == table && trigger.event == event) {
                result.push_back(trigger);
            }
        }
        return result;
    }

    std::string serialize() const {
        std::ostringstream oss;
        for (const auto& [name, trigger] : triggers) {
            oss << "#TRIGGER " << trigger.name << "\n";
            oss << trigger.tableName << " " << static_cast<int>(trigger.event) << "\n";
            oss << (trigger.condition.empty() ? "<none>" : trigger.condition) << "\n";
            for (const std::string& instr : trigger.instructions) {
                oss << instr << "\n";
            }
            oss << "#END_TRIGGER\n";
        }
        return oss.str();
    }

    void deserialize(const std::string& content) {
        std::istringstream iss(content);
        std::string line;
        Trigger current;
        bool reading = false;

        while (std::getline(iss, line)) {
            if (line.rfind("#TRIGGER ", 0) == 0) {
                current = Trigger();
                current.name = line.substr(9);
                reading = true;
            }
            else if (line == "#END_TRIGGER") {
                if (!current.name.empty()) {
                    triggers[current.name] = current;
                }
                reading = false;
            }
            else if (reading) {
                if (current.tableName.empty()) {
                    std::istringstream meta(line);
                    int evt;
                    meta >> current.tableName >> evt;
                    current.event = static_cast<EventType>(evt);
                }
                else if (current.condition.empty()) {
                    current.condition = (line == "<none>") ? "" : line;
                }
                else {
                    current.instructions.push_back(line);
                }
            }
        }
    }
    const std::vector<Trigger>& getAllTriggers() const;

private:
    std::unordered_map<std::string, Trigger> triggers;
    mutable std::vector<Trigger> cachedTriggers;
};
