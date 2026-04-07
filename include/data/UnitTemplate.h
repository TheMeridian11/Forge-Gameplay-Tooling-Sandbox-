#pragma once

#include <string>
#include <vector>

// Represent a unit definition loaded from external data.
// This is a pure data structure used before creating runtime unit objects
struct UnitTemplate {
    int id = 0;
    std::string name;
    std::string team;
    int max_health = 0;
    int current_health = 0;
    int attack_power = 0;
    std::vector<std::string> ability_IDs;
};