// So basically, this loader just does the following:
// opens the file, parses the json, checks the root is an array, reads entry into a UnitTemplate
// and finally returns all templates. 

#include "data/UnitLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
// This is just to help quikcly track values we have already seen.
#include <unordered_set>

#include "nlohmann/json.hpp"

// Create a shorter alias so we can write "json" instead of "nlohmann::json"
using json = nlohmann::json;


// Validate that a JSON object contains a required field.
static void validateRequiredField(const json& entry, const std::string& fieldName, std::size_t entryIndex) {
    if (!entry.contains(fieldName)) {
        throw std::runtime_error("Unit entry at index " + std::to_string(entryIndex) + " is missing required field: " + fieldName);
    }
}

// Validates that a field is an integer
static void validateIntegerField(const json& entry, const std::string& fieldName, std::size_t entryIndex) {
    validateRequiredField(entry, fieldName, entryIndex);

    if (!entry.at(fieldName).is_number_integer()) {
        throw std::runtime_error("Field " + fieldName + " in unit entry at index " + std::to_string(entryIndex) + " must be an integer.");
    }
}

// Validates that a field is a string.
static void validateStringField(const json& entry, const std::string& fieldName, std::size_t entryIndex) {
    validateRequiredField(entry, fieldName, entryIndex);

    if (!entry.at(fieldName).is_string()) {
        throw std::runtime_error("Field " + fieldName + " in unit entry at index " + std::to_string(entryIndex) + " must be a string.");
    }
}

// Validates that a field is an array of strings.
static void validateStringArrayField(const json& entry, const std::string& fieldName, std::size_t entryIndex) {
    validateRequiredField(entry, fieldName, entryIndex);

    if (!entry.at(fieldName).is_array()) {
        throw std::runtime_error("Field '" + fieldName + "' in unit entry at index " + std::to_string(entryIndex) + " must be an array.");
    }

    const json& valueArray = entry.at(fieldName);

    for (std::size_t i = 0; i < valueArray.size(); ++i) {
        if (!valueArray.at(i).is_string()) {
            throw std::runtime_error("Field '" + fieldName + "' in unit entry at index " + std::to_string(entryIndex) + " must contain only strings.");
        }
    }
}

// Validates the full structure and contents of a unit JSON entry... this one is big lol! 
static void validateUnitEntry(const json& entry, std::size_t entryIndex) {
    if (!entry.is_object()) {
        throw std::runtime_error("Unit entry at index " + std::to_string(entryIndex) + " must be a JSON object.");
    }

    // Validate required fields and types using functions created before hand.
    validateIntegerField(entry, "id", entryIndex);
    validateStringField(entry, "name", entryIndex);
    validateStringField(entry, "team", entryIndex);
    validateIntegerField(entry, "max_health", entryIndex);
    validateIntegerField(entry, "current_health", entryIndex);
    validateIntegerField(entry, "attack_power", entryIndex);
    validateStringArrayField(entry, "ability_IDs", entryIndex);

    // Extract values after type validation so we can validate gameplay function/simulation unit rules. 
    const int id = entry.at("id").get<int>();
    const std::string name = entry.at("name").get<std::string>();
    const std::string team = entry.at("team").get<std::string>();
    const int max_health = entry.at("max_health").get<int>();
    const int current_health = entry.at("current_health").get<int>();
    const int attack_power = entry.at("attack_power").get<int>();

    // Validate sensible gameplay values.
    if (id < 0) {
        throw std::runtime_error("Field 'id' in unit entry at index " + std::to_string(entryIndex) + " cannot be negative.");
    }

    if (name.empty()) {
        throw std::runtime_error("Field 'name' in unit entry at index " + std::to_string(entryIndex) + " cannot be empty");
    }

    if (team != "Blue" && team != "Red") {
        throw std::runtime_error("Field 'team' in unit entry at index " + std::to_string(entryIndex) + " must be either 'Blue' or 'Red'.");
    }

    if (max_health <= 0) {
        throw std::runtime_error("Field 'max_health' in unit entry at index " + std::to_string(entryIndex) + " must be greater than 0");
    }

    if (current_health < 0) {
        throw std::runtime_error("Field 'current_health' in unit entry at index " + std::to_string(entryIndex) + " cannot be negative...means he already dead lol");
    }

    if (current_health > max_health) {
        throw std::runtime_error("Field 'current_health' in unit entry at index " + std::to_string(entryIndex) + " cannot be greater than 'max_health'.");
    }

    if (attack_power < 0) {
        throw std::runtime_error(
            "Field 'attack_power' in unit entry at index " + std::to_string(entryIndex) + " cannot be negative.");
    }
}

// Validates the set of loaded unit templates after all files have been read.
// These checks catch problems that only appear when looking at all units together.
static void validateLoadedUnitTemplates(const std::vector<UnitTemplate>& unitTemplates) {
    // The manifest should produce at least one loaded unit.
    if (unitTemplates.empty()) {
        throw std::runtime_error("Unit manifest did not produce any loaded unit templates.");
    }

    std::unordered_set<int> seen_IDs;
    std::unordered_set<std::string> seen_names;

    for (const UnitTemplate& unitTemplate : unitTemplates) {
        // Ensure no duplicate unit ID's exist across different files.
        if (!seen_IDs.insert(unitTemplate.id).second) {
            throw std::runtime_error("Duplicate unit ID detected across loaded files: " + std::to_string(unitTemplate.id));
        }

        // Ensure no duplicate unit names exist across different files.
        if (!seen_names.insert(unitTemplate.name).second) {
            throw std::runtime_error("Duplicate unit name detected across loaded files: " + unitTemplate.name);
        }
    }
}

// Loads and validates a single unit file.
// Since each file contains one unit object, we validate it as entry index 0.
UnitTemplate UnitLoader::loadUnitTemplateFromFile(const std::string& filePath) {
    std::ifstream inputFile(filePath);

    // If the file could not be opened, stop immediately with a clear error
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open unit data file: " + filePath);
    }

    // This json object will hold the fully parsed contents of the file.    
    json rootJson;

    try {
        // Read and parse the file contents directly into the JSON object.        
        inputFile >> rootJson;
    } catch (const json::parse_error& error) {
        throw std::runtime_error("Failed parse JSON in unit data file '" + filePath + "': " + error.what());
    }

    // Validate the structure and values of the current entry before reading it.
    // we use 0 because this file contains just one unit object, not an array of many.
    validateUnitEntry(rootJson, 0);

    UnitTemplate unitTemplate;
    // Read each expected field from the JSON object.
    //
    // entry.at("fieldName") looks up a required field.
    // If the field is missing, it throws an error.
    //
    // .get<Type>() converts the JSON value into the C++ type we want.
    unitTemplate.id = rootJson.at("id").get<int>();
    unitTemplate.name = rootJson.at("name").get<std::string>();
    unitTemplate.team = rootJson.at("team").get<std::string>();
    unitTemplate.max_health = rootJson.at("max_health").get<int>();
    unitTemplate.current_health = rootJson.at("current_health").get<int>();
    unitTemplate.attack_power = rootJson.at("attack_power").get<int>();
    unitTemplate.ability_IDs = rootJson.at("ability_IDs").get<std::vector<std::string>>();
    
    return unitTemplate;
}

// Loads a manifest file and then loads each unit file listed inside it.
std::vector<UnitTemplate> UnitLoader::loadUnitTemplatesFromManifest(const std::string& manifestFilePath, const std::string& unitsDirectory) {
    // Open the JSON file for reading.
    std::ifstream inputFile(manifestFilePath);

    // If the file could not be opened, stop immediately with a clear error
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open unit manifest file: " + manifestFilePath);
    }

    // This json object will hold the fully parsed contents of the file.
    json manifestJson;

    try {
        // Read and parse the file contents directly into the JSON object.
        inputFile >> manifestJson; 
    } catch (const json::parse_error& error) {
        throw std::runtime_error("Failed to parse JSON in unit manifest file '" + manifestFilePath + "': " + error.what());
    }

    // I expect the root of the JSON file to be an array, like: 
    // [
    //   { ... },
    //   { ... },
    //   { ... }
    // ]
    // 
    // If the root is not an array, the file format is not what i expected duhhh.
    if (!manifestJson.is_array()) {
        throw std::runtime_error("Unit manifest file must contain a JSON array: " + manifestFilePath);
    }

    // Reject an empty manifest because it is almost certainly accidental and would otherwise produce a black simulation.
    if (manifestJson.empty()) {
        throw std::runtime_error("Unit manifest file cannot be empty: " + manifestFilePath);
    }

    // This vector will store all loaded unit templates after parsing.
    std::vector<UnitTemplate> unitTemplates;
    std::unordered_set<std::string> seen_file_names;

    // Loop through each JSON object in the array.
    for (std::size_t index = 0; index < manifestJson.size(); index++) {
        const json& entry = manifestJson.at(index);

        if (!entry.is_string()) {
            throw std::runtime_error("Manifest entry at index " + std::to_string(index) + " must be a string file name.");
        }

        const std::string fileName = entry.get<std::string>();

        // Reject duplicate file names inside the manifest.
        if (!seen_file_names.insert(fileName).second) {
            throw std::runtime_error("Duplicate file name found in unit manifest: " + fileName);
        }

        const std::string filePath = unitsDirectory + "/" + fileName;

        // Add the fully parsed template into the result list.
        unitTemplates.push_back(loadUnitTemplateFromFile(filePath));
    }

    // Validate the final set of loaded templates across all files.
    validateLoadedUnitTemplates(unitTemplates);

    // Return all successfully loaded unit templates.
    return unitTemplates;

}