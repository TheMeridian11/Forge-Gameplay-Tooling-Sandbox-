#include "ui/SimulationPanel.h"

#include <vector>

#include "imgui.h"

// PHASE 2
// Converts a team enum value into readable text for the UI.
const char* teamToString(Team team) {
    switch (team) {
        case Team::Blue:
            return "Blue";
        case Team::Red:
            return "Red";
        default:
            return "Unknown";
    }
}


// Converts pause state into readable text for the UI.
const char* simulationStatusToString(bool isPaused) {
    if (isPaused == true) {
        return "Paused";
    }
    return "Running";
}

// Draws the simulation debug panel and reports which actions the user requested.
SimulationPanelActions drawSimulationPanel(const SimulationState& simulation_state) {
    SimulationPanelActions action_panel;

    // Drawing a simple debug window
    // ̶p̶a̶r̶t̶i̶c̶u̶l̶a̶r̶l̶y̶,̶ ̶w̶e̶ ̶a̶r̶e̶ ̶c̶r̶e̶a̶t̶i̶n̶g̶ ̶a̶ ̶l̶i̶t̶t̶l̶e̶ ̶U̶I̶ ̶p̶a̶n̶e̶l̶ ̶t̶h̶a̶t̶ ̶p̶r̶o̶v̶e̶s̶ ̶I̶m̶G̶u̶i̶ ̶i̶s̶ ̶r̶e̶n̶d̶e̶r̶i̶n̶g̶.̶
    // This window will be showing the current simulation state.
    ImGui::Begin("Simulation Overview");
    ImGui::Text("Phase 2: Core simulation foundation");
    ImGui::Separator();

    ImGui::Text("Status: %s", simulationStatusToString(simulation_state.isPaused()));
    ImGui::Text("Total Units: %zu", simulation_state.getUnitCount());
    ImGui::Text("Tick Count: %zu", simulation_state.getTickCount());
    ImGui::Text("Elapsed Time: %.2f seconds", simulation_state.getElapsedTimeSeconds());

    ImGui::Spacing();
    ImGui::Text("Simulation Controls");

    if (simulation_state.isPaused()) {
        if (ImGui::Button("Resume Simulation")) {
            action_panel.requestResume = true;
        }
    } else {
        if (ImGui::Button("Pause Simulation")) {
            action_panel.requestPause = true;
        }
    }

    // Allow resetting the simulation back to its initial state.
    if (ImGui::Button("Reset Simulation")) {
        action_panel.requestReset = true;
    }

    // Allow advancing the simulation by exactly one fixed step while paused. 
    if (simulation_state.isPaused()) {
        if (ImGui::Button("Step Simulation")) {
            action_panel.requestStep = true;
        }
    }

    ImGui::Spacing();
    ImGui::Text("Units");

    const std::vector<Unit>& units = simulation_state.getUnits();

    for (const Unit& unit : units) {
        ImGui::Separator();
        ImGui::Text("Unit ID: %d", unit.id);
        ImGui::Text("Name: %s", unit.name.c_str());
        ImGui::Text("Team: %s", teamToString(unit.team));
        ImGui::Text("Health: %d / %d", unit.stats.current_health, unit.stats.max_health);
        ImGui::Text("Attack Power: %d", unit.stats.attack_power);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Event Log");

    const std::vector<std::string>& eventLog = simulation_state.getEventLog();

    if (eventLog.empty()) {
        ImGui::TextUnformatted("No events recorded yet.");
    } else {
        for (const std::string& entry : eventLog) {
            ImGui::BulletText("%s", entry.c_str());
        }
    }

    //ImGui::Text("Next: smoke test and first successful build, lol memes");
    ImGui::End();

    return action_panel;
}

