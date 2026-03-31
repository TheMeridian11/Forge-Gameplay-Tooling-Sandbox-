#include "simulation/SimulationState.h"

// Adds a unit to the internal unit list
void SimulationState::addUnit(const Unit& unit) {
    m_units.push_back(unit);
}

// Returns a read only reference to all units in the simulation
const std::vector<Unit>& SimulationState::getUnits() const {
    return m_units;
}

// Returns the number of units currently stored in the simulation.
std::size_t SimulationState::getUnitCount() const {
    return m_units.size();
}

