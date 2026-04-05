// So basically, this loader just does the following:
// opens the file, parses the json, checks the root is an array, reads entry into a UnitTemplate
// and finally returns all templates. 

#include "data/UnitLoader.h"

#include <fstream>
#include <stdexcept>

#include "nlohmann/json.hpp"

// Create a shorter alias so we can write "json" instead of "nlohmann::json"
using json = nlohmann::json;

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

    // Read and parse the file contents directly into the JSON object.
    // If the file contains invalid JSON syntax, this operation will fail.
    inputFile >> rootJson;

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
    for (const json& entry : rootJson) {
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

