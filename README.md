# Forge-Gameplay-Tooling-Sandbox-
This is a C++ gameplay systems sandbox where units, abilities, and status effects are simulated in real time, with developer tooling for inspection, debugging, and rapid iteration.

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

Phase 1 of development foundation is complete.

The project currently includes:
- C++20 project setup with CMake
- SDL2 window creation
- Dear ImGui integration
- a working application loop
- Catch2 test setup
- an initial smoke test (not actually testing smoke functionality but rather that the testing environment works)
- code formatting with clang-format

## Planned Features

The project will later expand to include:
- unit and ability definitions
- combat simulation systems
- cooldown and status effect logic
- event logging
- gameplay debugging tools
- data-driven content loading using JSON
- developer inspection panels for simulation state

## Tech Stack

- **Language:** C++20
- **Build System:** CMake
- **Windowing / Rendering:** SDL2
- **Debug UI:** Dear ImGui
- **Data Format:** JSON using nlohmann/json
- **Testing:** Catch2
- **Formatting:** clang-format

## Project Structure

```text
.
├── assets/
├── docs/
├── external/
├── include/
├── src/
├── tests/
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
- The current version is an early foundation build.
- Gameplay systems have not been implemented yet.
- The focus so far has been on creating a clean, professional starting point for future development.

