// Here the libraries we are including are pulling in:
// SDL, standard output for errors, ImGui and ImGui SDL integration code

#include <SDL2/SDL.h>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

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

    if (!window) {
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

    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Starting ImGui
    IMGUI_CHECKVERSION();
    // creating the imgui context
    ImGui::CreateContext();
    // getting the imgui i/o object
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // just setting a dark theme
    ImGui::StyleColorsDark();

    // Connecting ImGui to SDL
    // This is our glue/connection between the SDL window/input, the sdl renderer and ImGui
    // without these, ImGui would not know how to render or receive SDL events.
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    bool running = true;
    // This is the loop that will be keeping our application alive, running every frame until 
    // the user closes the window.
    while (running == true) {
        // Event handling
        // checking for things like window closing, keyboard input, mouse input
        // we also send SDL events into ImGui too.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Starting a new ImGui Frame
        // we are telling imgui here to build a new frame of UI
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Drawing a simple debug window
        // particularly, we are creating a little UI panel that proves ImGui is rendering.
        ImGui::Begin("Forge Debug");
        ImGui::Text("Week 1 setup is working.");
        ImGui::Text("SDL2 window and Dear ImGui are running!");
        ImGui::Separator();
        ImGui::Text("Next: smoke test and first successful build, lol memes");
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
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}