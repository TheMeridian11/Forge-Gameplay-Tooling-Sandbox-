#include "data/ContentPaths.h"

// Returns the root assets directory used by the project.
std::string ContentPaths::getAssetsRoot() {
    return "assets";
}

// Returns the directory that contains the unit data files.
std::string ContentPaths::getUnitsDirectory() {
    return getAssetsRoot() + "/units";
}

// Returns the default sample unit data file path.
std::string ContentPaths::getSampleUnitsFilePath() {
    return getUnitsDirectory() + "/sample_units.json";
}