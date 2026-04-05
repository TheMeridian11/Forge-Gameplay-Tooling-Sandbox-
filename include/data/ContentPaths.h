#pragma once

#include <string>

// Provide central access to important content/data file paths.
// This keeps file path decisions out of simulation and gameplay code.
class ContentPaths {
    public:
    // Returns the root assets directory used by the project.
    static std::string getAssetsRoot();

    // Returns the directory that contains unit data files.
    static std::string getUnitsDirectory();

    // Returns the unit manifest file path.
    static std::string getUnitManifestFilePath();
};