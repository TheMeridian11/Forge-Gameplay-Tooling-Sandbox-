#include "simulation/SimulationState.h"

// creates a starter simulation containing a few hardcoded example units.
// Later, this setup will likely be replaced by JSON-driven content loading.
SimulationState SimulationState::createSampleState() {
    SimulationState state;

    // for now, we are starting units below max health so that regeneration is visible in the UI
    state.addUnit(Unit{
        1,
        "Knight",
        Team::Blue,
        Stats{150, 100, 20, 0.0f}
    });

    state.addUnit(Unit{
        2,
        "Mage",
        Team::Red,
        Stats{200, 150, 40, 0.0f}
    });

    state.addUnit(Unit{
        3,
        "Archer",
        Team::Blue,
        Stats{100, 60, 15, 0.0f}
    });

    return state;
}

// Advances the simulation by one update step.
// For now, this only tracks time and tick count and pausing support.
// Later this function will also drive gameplay systems. 
void SimulationState::update(float deltaTimeSeconds) {
    // if the simulation is paused, no time or tick progress occurs.
    if (m_isPaused == true) {
        return;
    }

    ++m_tickCount;
    m_elapsedTimeSeconds += deltaTimeSeconds;

    // Apply a very small passive health regeneration effect to each unit.
    // This is my first example of per unit simulation behaviour.
    // NOTE: the 'constexpr' is used for when you want to enable a variable
    // (or a function) return values to be computed at compile time rather than runtime.
    constexpr float health_regen_per_second = 5.0f;

    // If already at max health, do nothing
    // otherwise add fractional regen progress 
    // once enough progress builds up, increase health by whole numbers.
    for (Unit& unit : m_units) {
        // Skip units that are already fully healed
        if (unit.stats.current_health >= unit.stats.max_health) {
            unit.stats.regeneration_progress = 0.0f;
            continue;
        }

        // Accumulate fractional regeneration over time.
        unit.stats.regeneration_progress += health_regen_per_second * deltaTimeSeconds;

        // convert whole number regeneration progress into actual health.
        while (unit.stats.regeneration_progress >= 1.0f && unit.stats.current_health < unit.stats.max_health) {
            ++unit.stats.current_health;
            unit.stats.regeneration_progress -= 1.0f;
        }

        // Clamp health to its maximum value.
        if (unit.stats.current_health > unit.stats.max_health) {
            unit.stats.current_health = unit.stats.max_health;
            unit.stats.regeneration_progress = 0.0f;
        }
    }
}

// Pauses the simulation
void SimulationState::pause() {
    m_isPaused = true;
}

// Resume the simulation.
void SimulationState::resume() {
    m_isPaused = false;
}

// Toggles between paused and running states.
void SimulationState::togglePaused() {
    if (m_isPaused == true) {
        m_isPaused = false;
    } else {
        m_isPaused = true;
    }
}

bool SimulationState::isPaused() const {
    return m_isPaused;
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

// Returns the number of simulation ticks that have occured.
std::size_t SimulationState::getTickCount() const {
    return m_tickCount;
}

// Returns the total elapsed simulation time in seconds.
float SimulationState::getElapsedTimeSeconds() const {
    return m_elapsedTimeSeconds;
}

