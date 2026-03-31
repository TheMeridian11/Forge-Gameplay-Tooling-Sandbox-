#pragma once

#include <vector>

#include "gameplay/Unit.h"

// Holds the current state of the sandbox simulation
// For phase 2, it mainly stores units and allows new ones to be added. 
class SimulationState {
    public:
    // Adds a unit into the simulation
    void addUnit(const Unit& unit);

    // Returns all units currently in the simulation.
    const std::vector<Unit>& getUnits() const;

    // Returns the total number of units in the simulation.
    std::size_t getUnitCount() const;

    private:
    std::vector<Unit> m_units;
};