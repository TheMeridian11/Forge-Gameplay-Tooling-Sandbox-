#pragma once

#include <string>

#include "gameplay/Stats.h"

// This class represents which side a unit belongs to in the simulation
enum class Team {
    Blue,
    Red
};

// This struct will represent a single unit/entity in the sandbox.
// For now, a unit only contains a few essential fields.
// Later, this can expand with cooldowns, effects, resources, position and more.
struct Unit {
    int id = 0;
    std::string name;
    Team team = Team::Blue;
    Stats stats;
};