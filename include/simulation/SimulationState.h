#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "data/UnitTemplate.h"
#include "gameplay/Unit.h"

// following good OOP/Encapsulation, (public vs private)
// I wnated to hide internal data and only expose the operations we want others to use.
// this makes the class easier to manage as it grows.

// I also would like to clarify the different ways 'const' is used here.
// for example:
// (1) const Unit& unit:
// This parameter is read only.
// (2) const std::vector<Unit>&:
// The returned reference is read only
// (3) getUnits() const:
// The member fucntion itself is read only with respect to the object 


// Holds the current state of the sandbox simulation
// At this stage, it stores units, tracks timing data, and exposes
// simple control/debug functionality for the sandbox. 
class SimulationState {
    public:

    // creates a starter simulation state with a few built in sample units.
    // This is useful during early development before JSON-driven loading exists.
    static SimulationState createSampleState();

    // Advances the simulation by the given delta time in seconds.
    // (will be called from main.cpp every frame)
    void update(float deltaTimeSeconds);

    // Advances the simulation by a single step, even if the simulation is paused.
    void stepOnce(float deltaTimeSeconds);

    // Reset the simulation back to its initial sample state.
    void reset();

    // Pauses the simulation updates.
    void pause();

    // Resumes the simulation updates.
    void resume();

    // Toggles between paused and runnin states.
    void togglePaused();

    // Return whether the simulation is currently paused or not.
    bool isPaused() const;

    // Adds a unit into the simulation
    void addUnit(const Unit& unit);

    // Returns all units currently in the simulation.
    const std::vector<Unit>& getUnits() const;

    // Returns the most recent event log entries as a read only view.
    const std::vector<std::string>& getEventLog() const;

    // Returns the total number of units in the simulation.
    std::size_t getUnitCount() const;

    //Returns the total number of simulation ticks that have occured.
    std::size_t getTickCount() const;

    // Returns the total elapsed simulation time in seconds.
    float getElapsedTimeSeconds() const;

    // (1) private functions
    private:
    // Applies all per frame simulation logic.
    void applySimulationStep(float deltaTimeSeconds);

    // Adds a text entry to the event log.
    void logEvent(const std::string& message);

    // Converts a loaded unit template into runtime unit instance.
    static Unit createUnitFromTemplate(const UnitTemplate& UnitTemplate);

    // (2) private variables
    private:
    // m_units is private because it is the internal state of the class
    // we do not want outside code doing anything it wants directly to this vector.
    std::vector<Unit> m_units;

    std::vector<std::string> m_eventLog;

    // Counts how many update ticks have occured.
    // (how many updates have happened essentially)
    std::size_t m_tickCount = 0;

    // Tracks the total elapsed simulation time.
    float m_elapsedTimeSeconds = 0.0f;

    // Controls whether the simulation is currently paused or not.
    bool m_isPaused = false;
};