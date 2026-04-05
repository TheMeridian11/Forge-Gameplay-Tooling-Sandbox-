// Here the libraries we are including are pulling in:
// SDL, standard output for errors, ImGui and ImGui SDL integration code

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "simulation/SimulationState.h"
#include "ui/SimulationPanel.h"


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

        // Advance the simulation normally when it is not paused
        simulation_state.update(deltaTimeSeconds);

        // Starting a new ImGui Frame
        // we are telling imgui here to build a new frame of UI
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Draw the simulation panel and collect any requested actions.
        const SimulationPanelActions actions_panel = drawSimulationPanel(simulation_state);

        if (actions_panel.requestPause == true) {
            simulation_state.pause();
        }

        if (actions_panel.requestResume == true) {
            simulation_state.resume();
        }

        if (actions_panel.requestReset == true) {
            simulation_state.reset();
        }

        if (actions_panel.requestStep == true) {
            simulation_state.stepOnce(manual_step_delta_time);
        }

        // Now we actually finalise and render the UI frame and everything else.
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