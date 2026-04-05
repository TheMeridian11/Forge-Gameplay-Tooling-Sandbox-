# Forge-Gameplay-Tooling-Sandbox-
This is a C++ gameplay systems sandbox where units, abilities, and status effects are simulated in real time, with developer tooling for inspection, debugging, rapid iteration
and clean software architecture.

## Overview

Forge Gameplay Tooling Sandbox is a personal C++ project designed to simulate small scale gameplay systems in a structured and extensible way. The long term goal is to build a sandbox where units, abilities, cooldowns, status effects, and combat interactions can be tested and inspected through developer-facing tools.

This project is being developed as a portfolio piece for myself to showcase my:
- modern C++ development
- clean project architecture
- build system setup with CMake
- automated testing with Catch2
- developer tooling using Dear ImGui
- data-driven design for gameplay systems

## Current Status

Phase 1 and 2 of development is complete.

The project currently includes:
- C++20 project setup with CMake
- SDL2 window creation
- Dear ImGui integration
- Catch2 test setup
- core gameplay data structures (`Stats`, `Unit`)
- simulation state management
- simulation tick and elapsed time tracking
- JSON-based sample unit loading
- validation and safer error handling for unit JSON content
- centralized content path helper for data file locations
- pause, resume, reset, and single-step simulation controls
- passive per-unit health regeneration over time
- dedicated simulation panel UI module
- event/debug log for simulation actions
- code formatting with clang-format


## Current Features

### Simulation
- sample units are created in a starter simulation state
- unit content loaded from multiple JSON files via a manifest
- simulation updates over time using delta time
- tick count and elapsed time are tracked
- units regenerate health over time
- simulation can be paused, resumed, reset, or stepped manually

### Tooling / Debug UI
- simulation overview panel
- unit inspection readout
- simulation controls
- event/debug log showing recent actions

## Planned Features for Next Phase
Phase 3 will introduce data-driven loading so that simulation content is no longer hardcoded in C++. The next planned additions include:
- JSON-defined unit data
- data loading/parsing layer
- validation of loaded content
- moving sample simulation setup into external data files

## Tech Stack

- **Language:** C++20
- **Build System:** CMake
- **Windowing / Rendering:** SDL2
- **Debug UI:** Dear ImGui
- **Data Format:** JSON using nlohmann/json
- **Testing:** Catch2
- **Formatting:** clang-format
- **CI:** GitHub Actions

## Project Structure

```text
.
├── assets/
├── docs/
├── external/
├── include/
│   ├── gameplay/
│   ├── simulation/
│   └── ui/
├── src/
│   ├── simulation/
│   └── ui/
├── tests/
├── .github/
├── CMakeLists.txt
└── README.md
```
# Build Instructions
From the project root:
> cmake -B build -S .
> cmake --build build

# Run the Application
> ./build/forge_gameplay_tooling_sandbox

# Run Tests
> ctest --test-dir build --output-on-failure

## Notes:
- The current version focuses on simulation foundations and debug tooling.
- Gameplay systems have not been implemented yet.
- The next phase will transition the project toward data-driven loading.

