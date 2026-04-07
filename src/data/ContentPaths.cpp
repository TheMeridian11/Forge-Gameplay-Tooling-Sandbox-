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
std::string ContentPaths::getUnitManifestFilePath() {
    return getUnitsDirectory() + "/unit_manifest.json";
}

//  below two functions work the same as the two unit ones except for abilities lol
std::string ContentPaths::getAbilitiesDirectory() {
    return getAssetsRoot() + "/abilities";
}

std::string ContentPaths::getAbilityManifestFilePath() {
    return getAbilitiesDirectory() + "/ability_manifest.json";
}