# This line is basically stating that this project can only be built with CMake version 3.20 or higher
# We do this to avoid weird compatibility issues.
cmake_minimum_required(VERSION 3.20)

# Project Definition
project(ForgeGameplaySandbox VERSION 0.1.0 LANGUAGES CXX)

# use c++20, do not fall back to an older standard and do not use compiler specific language extensions
# unless necessary (last line will allow for a project thats more clean and portable)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# This creates a toggle where:
# ON = build test files
# OFF = skip tests
option(BUILD_TESTS "Build tests" ON)

# This will tell our cmake file to look for the following packages
# If the library happens to be missing, we stop and fail the configure step
find_package(SDL2 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Catch2 3 REQUIRED)

# Pointing to the ImGui folder
# Here we are creating a variable called "IMGUI_DR" that points to our vendored imgui folder
# "CMAKE_SOURCE_DIR" just means the root of the project ---> so this would look like "the-project/external/imgui"
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

# Because we are using the imgui resources as part of our implementation, we cant just ignore them when they are in use
# Thus, we need to make sure we are also compiling the key files that we are actually using within our project
# (the backend files are the SDL specific integration pieces)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

# We are building an executable called forge_gameplay_tooling_sandbox
# this executable will use the main file and the relevant imgui files
# This is essentially our actual application.
add_executable(forge_gameplay_tooling_sandbox
    src/main.cpp
    src/data/ContentPaths.cpp
    src/data/UnitLoader.cpp
    src/simulation/SimulationState.cpp
    src/ui/SimulationPanel.cpp
    ${IMGUI_SOURCES}
)

# Including directories
# We are telling the compiler where to look when we write '#include'
# for example: 
# 1) 'include' lets us include our own headers later
# 2) '${IMGUI_DIR}' lets us include 'imgui.h'
# 3) '${IMGUI_DIR}/backends' lets us include backend headers like 'imgui_impl_sdl2.h' 
target_include_directories(forge_gameplay_tooling_sandbox PRIVATE
    include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Linking Libraries
# here we are telling cmake that the executable depends on SDL2 and nlohmann/json
# even if we include headers, our program will still often need to link witrh the actual library too
target_link_libraries(forge_gameplay_tooling_sandbox PRIVATE
    SDL2::SDL2
    nlohmann_json::nlohmann_json
)

# TEST SETUP
# The block of code here is saying:
# -If the test building is enabled:
if(BUILD_TESTS)
    # turn on testing
    enable_testing()

    # build a test executable called 'forge_tests'
    add_executable(forge_tests
        # compile tests/test_smoke.cpp
        tests/test_smoke.cpp
    )

    # link it with catch2
    target_link_libraries(forge_tests PRIVATE
        Catch2::Catch2WithMain
    )

    # let CTest discover the tests automatically
    include(CTest)
    include(Catch)
    catch_discover_tests(forge_tests)
endif()



