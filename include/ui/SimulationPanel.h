#pragma once

#include "simulation/SimulationState.h"

// Represent the actions the UI can request from the simulation.
// The panel only reports user intent. Main.cpp will decide how to actually apply it.
struct SimulationPanelActions {
    bool requestPause = false;
    bool requestResume = false;
    bool requestReset = false;
    bool requestStep = false;
};

// Renders the main simulation debug panel and returns any user actions
// requested during this frame.
SimulationPanelActions drawSimulationPanel(const SimulationState& simulation_state);