// Here the libraries we are including are pulling in:
// SDL, standard output for errors, ImGui and ImGui SDL integration code

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "simulation/SimulationState.h"

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

int main(int argc, char* argv[]) {
    // we silence warning when these parameters remain unused
    (void)argc;
    (void)argv;

    // Initialising SDL
    // This starts the SDL subsystems we need
    // If it fails, the program prints an error and exits (doing this for good practice)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    // Creating a window
    // the visible window we create will have a title of "Forge_gameplay_tooling_sandbox"
    // a size of 1280/720 and to be centered on the screen
    SDL_Window* window = SDL_CreateWindow(
        "Forge Gameplay Tooling Sandbox",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    // Creating the renderer
    // This creates the rendewrer used to draw to the window
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        // the extra flags added here are:
        // 1) SDL_RENDERER_ACCELERATED = use hardware acceleration if available
        // 2) SDL_RENDERER_PRESENTVSYNC = sync rendering to monitor refresh rate
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Starting ImGui
    IMGUI_CHECKVERSION();
    // creating the imgui context
    ImGui::CreateContext();
    // just setting a dark theme
    ImGui::StyleColorsDark();

    // Connecting ImGui to SDL
    // This is our glue/connection between the SDL window/input, the sdl renderer and ImGui
    // without these, ImGui would not know how to render or receive SDL events.
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // PHASE 2
    // Create the first simulation state for the sandbox.
    SimulationState simulation_state = SimulationState::createSampleState();

    // Track frame timing so we can compute delta time each update
    // SDL_GetPerformanceCounter() provides us a high precision timer value.
    Uint64 previousCounter = SDL_GetPerformanceCounter();
    // the frequency function tells us how many timer counts happen per second.
    const Uint64 frequency = SDL_GetPerformanceFrequency();

    // Fixed single step size used for manual stepping while paused.
    constexpr float manual_step_delta_time = 1.0f / 60.0f;

    bool running = true;
    // This is the loop that will be keeping our application alive, running every frame until 
    // the user closes the window.
    while (running == true) {
        // Window event handling and processing inputs
        // checking for things like window closing, keyboard input, mouse input
        // we also send SDL events into ImGui too.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Calculate frame delta time in seconds.
        const Uint64 currentCounter = SDL_GetPerformanceCounter();
        // here we subtract to obtain how much time passed between frames.
        const Uint64 counterElapsed = currentCounter - previousCounter;
        previousCounter = currentCounter;

        // using the frequency function before hand, this division gives us frame time in seconds.
        // NOTE: The reasoning for using delta time:
        // because simulations should update based on "time passed", not just blindly assume each frame is equal.
        // Later this will matter for things like cooldown timers, effect durations, movement and combat timing. 
        // so i thought it would be a good idea to start this right habit from now in development.  
        const float deltaTimeSeconds = static_cast<float>(counterElapsed) / static_cast<float>(frequency);

        // Advance the simulation. 
        simulation_state.update(deltaTimeSeconds);

        // Starting a new ImGui Frame
        // we are telling imgui here to build a new frame of UI
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Drawing a simple debug window
        // ̶p̶a̶r̶t̶i̶c̶u̶l̶a̶r̶l̶y̶,̶ ̶w̶e̶ ̶a̶r̶e̶ ̶c̶r̶e̶a̶t̶i̶n̶g̶ ̶a̶ ̶l̶i̶t̶t̶l̶e̶ ̶U̶I̶ ̶p̶a̶n̶e̶l̶ ̶t̶h̶a̶t̶ ̶p̶r̶o̶v̶e̶s̶ ̶I̶m̶G̶u̶i̶ ̶i̶s̶ ̶r̶e̶n̶d̶e̶r̶i̶n̶g̶.̶
        // This window will be showing the current simulation state.
        ImGui::Begin("Simulation Overview");
        ImGui::Text("week 2 setup: Core simulation foundation");
        ImGui::Text("SDL2 window and Dear ImGui are running!");
        ImGui::Separator();
        ImGui::Text("Status: %s", simulationStatusToString(simulation_state.isPaused()));
        ImGui::Text("Total Units: %zu",  simulation_state.getUnitCount());
        ImGui::Text("Tick Count: %zu", simulation_state.getTickCount());
        ImGui::Text("Elapsed Time: %.2f seconds", simulation_state.getElapsedTimeSeconds());

        ImGui::Spacing();
        ImGui::Text("Simulation Controls");

        if (simulation_state.isPaused()) {
            if (ImGui::Button("Resume Simulation")) {
                simulation_state.resume();
            }
        } else {
            if (ImGui::Button("Pause Simulation")) {
                simulation_state.pause();
            }
        }

        // Allow resetting the simulation back to its initial state.
        if (ImGui::Button("Reset Simulation")) {
            simulation_state.reset();
        }

        // Allow advancing the simulation by exactly one fixed step while paused. 
        if (simulation_state.isPaused()) {
            if (ImGui::Button("Step Simulation")) {
                simulation_state.stepOnce(manual_step_delta_time);
            }
        }

        const std::vector<Unit>& units = simulation_state.getUnits();

        for (const Unit& unit : units) {
            ImGui::Separator();
            ImGui::Text("Unit ID: %d", unit.id);
            ImGui::Text("Name: %s", unit.name.c_str());
            ImGui::Text("Team: %s", teamToString(unit.team));
            ImGui::Text("Health: %d / %d", unit.stats.current_health, unit.stats.max_health);
            ImGui::Text("Attack Power: %d", unit.stats.attack_power);
        }
        //ImGui::Text("Next: smoke test and first successful build, lol memes");
        ImGui::End();

        // Now we actually render everything.
        ImGui::Render();
        // Finalise the imgui frame, clearing the screen to a dark colour, drawing the UI and presents
        // the frame to the window.
        SDL_SetRenderDrawColor(renderer, 18, 18, 24, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Clean shutdown
    // when the program exits, it cleans up:
    // Imgui Backend, imgui context, sdl renderer, sdl window and sdl itself
    // (Good practice to avoid messy shutdown behaviour).
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    // releasing sdl resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}