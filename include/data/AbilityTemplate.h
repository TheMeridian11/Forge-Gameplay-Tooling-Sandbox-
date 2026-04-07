#pragma once

#include <string>

// Represents a single ability definition loaded from external JSON data.
struct AbilityTemplate {
    std::string id;
    std::string name;
    int damage = 0;
    float cooldownSeconds = 0.0f;
};