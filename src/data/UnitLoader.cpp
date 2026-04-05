// So basically, this loader just does the following:
// opens the file, parses the json, checks the root is an array, reads entry into a UnitTemplate
// and finally returns all templates. 

#include "data/UnitLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

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

    // Extract values after type validation so we can validate gameplay function/simulation unit rules. 
    const int id = entry.at("id").get<int>();
    const std::string team = entry.at("team").get<std::string>();
    const int max_health = entry.at("max_health").get<int>();
    const int current_health = entry.at("current_health").get<int>();
    const int attack_power = entry.at("attack_power").get<int>();

    // Validate sensible gameplay values.
    if (id < 0) {
        throw std::runtime_error("Field 'id' in unit entry at index " + std::to_string(entryIndex) + " cannot be negative.");
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

// Loads unit templates from a JSON file on disk
std::vector<UnitTemplate> UnitLoader::loadUnitTemplatesFromFile(const std::string& filePath) {
    // Open the JSON file for reading.
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
        throw std::runtime_error("Failed to parse JSON in unit data file '" + filePath + "': " + error.what());
    }

    // I expect the root of the JSON file to be an array, like: 
    // [
    //   { ... },
    //   { ... },
    //   { ... }
    // ]
    // 
    // If the root is not an array, the file format is not what i expected duhhh.
    if (!rootJson.is_array()) {
        throw std::runtime_error("Unit data file must contain a JSON array: " + filePath);
    }

    // This vector will store all loaded unit templates after parsing.
    std::vector<UnitTemplate> unitTemplates;

    // Loop through each JSON object in the array.
    for (std::size_t index = 0; index < rootJson.size(); index++) {
        const json& entry = rootJson.at(index);

        // Validate the structure and values of the current entry before reading it.
        validateUnitEntry(entry, index);

        UnitTemplate unitTemplate;

        // Read each expected field from the JSON object.
        //
        // entry.at("fieldName") looks up a required field.
        // If the field is missing, it throws an error.
        //
        // .get<Type>() converts the JSON value into the C++ type we want.
        unitTemplate.id = entry.at("id").get<int>();
        unitTemplate.name = entry.at("name").get<std::string>();
        unitTemplate.team = entry.at("team").get<std::string>();
        unitTemplate.max_health = entry.at("max_health").get<int>();
        unitTemplate.current_health = entry.at("current_health").get<int>();
        unitTemplate.attack_power = entry.at("attack_power").get<int>();

        // Add the fully parsed template into the result list.
        unitTemplates.push_back(unitTemplate);
    }

    // Return all successfully loaded unit templates.
    return unitTemplates;

}

