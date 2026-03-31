#include "simulation/SimulationState.h"

// creates a starter simulation containing a few hardcoded example units.
// Later, this setup will likely be replaced by JSON-driven content loading.
SimulationState SimulationState::createSampleState() {
    SimulationState state;

    state.addUnit(Unit{
        1,
        "Knight",
        Team::Blue,
        Stats{150, 150, 20}
    });

    state.addUnit(Unit{
        2,
        "Mage",
        Team::Red,
        Stats{200, 200, 40}
    });

    state.addUnit(Unit{
        3,
        "Archer",
        Team::Blue,
        Stats{100, 100, 15}
    });

    return state;
}


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

