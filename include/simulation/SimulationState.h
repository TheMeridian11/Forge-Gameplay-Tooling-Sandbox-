#pragma once

#include <vector>

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

// PHASE 2 CONTINUATION:
// this is a new system i want to introduce which is a tick system that updates system code with each frame


// Holds the current state of the sandbox simulation
// For phase 2, it mainly stores units and allows new ones to be added. 
class SimulationState {
    public:

    // creates a starter simulation state with a few built in sample units.
    // This is useful during early development before JSON-driven loading exists.
    static SimulationState createSampleState();

    // Adds a unit into the simulation
    void addUnit(const Unit& unit);

    // Returns all units currently in the simulation.
    const std::vector<Unit>& getUnits() const;

    // Returns the total number of units in the simulation.
    std::size_t getUnitCount() const;

    // m_units is private because it is the internal state of the class
    // we do not want outside code doing anything it wants directly to this vector.
    private:
    std::vector<Unit> m_units;
};