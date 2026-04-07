#include "simulation/SimulationState.h"

#include <stdexcept>
#include <unordered_set>

#include "data/AbilityLoader.h"
#include "data/UnitLoader.h"
#include "data/ContentPaths.h"

// Creates a starter simulation by loading sample units and abilities from JSON data.
SimulationState SimulationState::createSampleState() {
    SimulationState state;

    const std::vector<UnitTemplate> UnitTemplates = 
        UnitLoader::loadUnitTemplatesFromManifest(ContentPaths::getUnitManifestFilePath(), ContentPaths::getUnitsDirectory());
    
    const std::vector<AbilityTemplate> abilityTemplates = 
        AbilityLoader::loadAbilityTemplatesFromManifest(ContentPaths::getAbilityManifestFilePath(), ContentPaths::getAbilitiesDirectory());

    validateUnitAbilityReferences(UnitTemplates, abilityTemplates);

    state.m_abilities = abilityTemplates;

    for (const UnitTemplate& unitTemplate : UnitTemplates) {
        state.addUnit(createUnitFromTemplate(unitTemplate));
    }
    
    state.logEvent("Simulation created from JSON manifest data.");
    return state;
}

// Converts a loaded data template into a runtime Unit object.
Unit SimulationState::createUnitFromTemplate(const UnitTemplate& unitTemplate) {
    Team team = Team::Blue;

    if (unitTemplate.team == "Blue") {
        team = Team::Blue;
    } else if (unitTemplate.team == "Red") {
        team = Team::Red;
    } else {
        throw std::runtime_error("Unknown team value in unit template: " + unitTemplate.team);
    }

    return Unit{
        unitTemplate.id,
        unitTemplate.name,
        team,
        Stats{
            unitTemplate.max_health,
            unitTemplate.current_health,
            unitTemplate.attack_power,
            0.0f
        },
        unitTemplate.ability_IDs
    };

}

// Validates thaty every ability ID referenced by eahc unit exists in the loaded ability data
void SimulationState::validateUnitAbilityReferences(const std::vector<UnitTemplate>& unitTemplates, const std::vector<AbilityTemplate>& abilityTemplates) {
    std::unordered_set<std::string> validAbilityIDs;

    for (const AbilityTemplate& abilityTemplate : abilityTemplates) {
        validAbilityIDs.insert(abilityTemplate.id);
    }

    for (const UnitTemplate& unitTemplate : unitTemplates) {
        for (const std::string& abilityId : unitTemplate.ability_IDs) {
            if (validAbilityIDs.find(abilityId) == validAbilityIDs.end()) {
                throw std::runtime_error("Unit '" + unitTemplate.name + "' references unknown ability id: " + abilityId);
            }
        }
    }
}

// Advances the simulation by one update step.
// For now, this only tracks time and tick count and pausing support.
// Later this function will also drive gameplay systems. 
void SimulationState::update(float deltaTimeSeconds) {
    // if the simulation is paused, no time or tick progress occurs.
    if (m_isPaused == true) {
        return;
    }

    applySimulationStep(deltaTimeSeconds);
}

// Advances the simulation by exactly one step, even if paused.
void SimulationState::stepOnce(float deltaTimeSeconds) {
    applySimulationStep(deltaTimeSeconds);
    logEvent("Manual simulation step exectued.");
}

// Resets the simulation back to the original same setup
void SimulationState::reset() {
    // this line is basically us saying that, replace this current simulation object
    // with a fresh new sample simulation
    // NOTE: later, when the simulation setup becomes data driven, i may do something more advanced
    // but this is perfect just for nows usage.
    *this = SimulationState::createSampleState();
    logEvent("Simulation reset!");
}

// Applies all per frame simulation behaviour.
void SimulationState::applySimulationStep(float deltaTimeSeconds) {
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
    logEvent("Simulation is now paused!");
}

// Resume the simulation.
void SimulationState::resume() {
    m_isPaused = false;
    logEvent("Simulation has been resumed!");
}

// Toggles between paused and running states.
void SimulationState::togglePaused() {
    if (m_isPaused == true) {
        m_isPaused = false;
        logEvent("Simulation resumed!");
    } else {
        m_isPaused = true;
        logEvent("Simulation paused");
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

// Returns all abilities currently "usable" in the simulation
const std::vector<AbilityTemplate>& SimulationState::getAbilities() const {
    return m_abilities;
}

// Returns the most recent event log entries.
const std::vector<std::string>& SimulationState::getEventLog() const {
    return m_eventLog;
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

// Adds an entry to the event log and trims it to a small recent history
void SimulationState::logEvent(const std::string& message) {
    constexpr std::size_t max_log_entries = 8;

    m_eventLog.push_back(message);

    if (m_eventLog.size() > max_log_entries) {
        m_eventLog.erase(m_eventLog.begin());
    }
}