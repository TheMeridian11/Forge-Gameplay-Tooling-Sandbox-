// So basically, this loader just does the following:
// opens the file, parses the json, checks the root is an array, reads entry into a AbilityTemplate
// and finally returns all templates. 

#include "data/AbilityLoader.h"

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
        throw std::runtime_error("Ability entry at index " + std::to_string(entryIndex) + " is missing required field: " + fieldName);
    }
}

// Validates that a field is an integer
static void validateIntegerField(const json& entry, const std::string& fieldName, std::size_t entryIndex) {
    validateRequiredField(entry, fieldName, entryIndex);

    if (!entry.at(fieldName).is_number_integer()) {
        throw std::runtime_error("Field " + fieldName + " in ability entry at index " + std::to_string(entryIndex) + " must be an integer.");
    }
}

// Validates that a field is a string.
static void validateStringField(const json& entry, const std::string& fieldName, std::size_t entryIndex) {
    validateRequiredField(entry, fieldName, entryIndex);

    if (!entry.at(fieldName).is_string()) {
        throw std::runtime_error("Field " + fieldName + " in ability entry at index " + std::to_string(entryIndex) + " must be a string.");
    }
}

// Validates that a field is a number.
static void validateNumberField(const json& entry, const std::string& fieldName, std::size_t entryIndex) {
    validateRequiredField(entry, fieldName, entryIndex);

    if (!entry.at(fieldName).is_number()) {
        throw std::runtime_error("Field '" + fieldName + "' in ability entry at index " + std::to_string(entryIndex) + " must be numeric.");
    }
}

// Validates a single ability JSON object.
static void validateAbilityEntry(const json& entry, std::size_t entryIndex) {
    if (!entry.is_object()) {
        throw std::runtime_error("Ability entry at index " + std::to_string(entryIndex) + " must be a JSON object.");
    }

    // Validate required fields and types using functions created before hand.
    validateStringField(entry, "id", entryIndex);
    validateStringField(entry, "name", entryIndex);
    validateIntegerField(entry, "damage", entryIndex);
    validateNumberField(entry, "cooldownSeconds", entryIndex);
    validateNumberField(entry, "range", entryIndex);
    validateStringField(entry, "description", entryIndex);

    // Extract values after type validation so we can validate gameplay function/simulation ability rules.
    const std::string id = entry.at("id").get<std::string>();
    const std::string name = entry.at("name").get<std::string>();
    const int damage = entry.at("damage").get<int>();
    const float cooldownSeconds = entry.at("cooldownSeconds").get<float>();
    const float range = entry.at("range").get<float>();
    const std::string description = entry.at("description").get<std::string>();

    // Validate sensible gameplay values.
    if (id.empty()) {
        throw std::runtime_error(
            "Field 'id' in ability entry at index " + std::to_string(entryIndex) + " cannot be empty.");
    }

    if (name.empty()) {
        throw std::runtime_error(
            "Field 'name' in ability entry at index " + std::to_string(entryIndex) + " cannot be empty.");
    }

    if (damage < 0) {
        throw std::runtime_error(
            "Field 'damage' in ability entry at index " + std::to_string(entryIndex) + " cannot be negative.");
    }

    if (cooldownSeconds < 0.0f) {
        throw std::runtime_error("Field 'cooldownSeconds' in ability entry at index " + std::to_string(entryIndex) + " cannot be negative.");
    }

    if (range < 0.0f) {
        throw std::runtime_error("Field 'range' in ability entry at index " + std::to_string(entryIndex) + " cannot be negative.");
    }

    if (description.empty()) {
        throw std::runtime_error("Field 'description' in ability entry at index " + std::to_string(entryIndex) + " cannot be empty");
    }
}

// Validates the set of loaded ability templates across multiple files.
// These checks catch problems that only appear when looking at all abilities together
static void validateLoadedAbilityTemplates(const std::vector<AbilityTemplate>& abilityTemplates) {
    // The manifest should produce at least one loaded unit.
    if (abilityTemplates.empty()) {
        throw std::runtime_error("Ability manifest did not produce any loaded ability templates.");
    }

    std::unordered_set<std::string> seen_IDs;
    std::unordered_set<std::string> seen_names;

    for (const AbilityTemplate& abilityTemplate : abilityTemplates) {
        // Ensure no duplicate ability ID's exist across different files.
        if (!seen_IDs.insert(abilityTemplate.id).second) {
            throw std::runtime_error("Duplicate ability id detected across loaded files: " + abilityTemplate.id);
        }

        // Ensure no duplicate ability names exist across different files.
        if (!seen_names.insert(abilityTemplate.name).second) {
            throw std::runtime_error("Duplicate ability name detected across loaded files: " + abilityTemplate.name);
        }
    }
}

// Loads and validates one ability JSON file.
AbilityTemplate AbilityLoader::loadAbilityTemplateFromFile(const std::string& filePath) {
    std::ifstream inputFile(filePath);

    // If the file could not be opened, stop immediately with a clear error
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open ability data file: " + filePath);
    }

    // This json object will hold the fully parsed contents of the file.
    json rootJson;

    try {
        // Read and parse the file contents directly into the JSON object.
        inputFile >> rootJson;
    } catch (const json::parse_error& error) {
        throw std::runtime_error("Failed to parse JSON in ability data file '" + filePath + "': " + error.what());
    }

    // Validate the structure and values of the current entry before reading it.
    // we use 0 because this file contains just one ability object, not an array of many.
    validateAbilityEntry(rootJson, 0);

    AbilityTemplate abilityTemplate;
    // Read each expected field from the JSON object.
    //
    // entry.at("fieldName") looks up a required field.
    // If the field is missing, it throws an error.
    //
    // .get<Type>() converts the JSON value into the C++ type we want.
    abilityTemplate.id = rootJson.at("id").get<std::string>();
    abilityTemplate.name = rootJson.at("name").get<std::string>();
    abilityTemplate.damage = rootJson.at("damage").get<int>();
    abilityTemplate.cooldownSeconds = rootJson.at("cooldownSeconds").get<float>();
    abilityTemplate.range = rootJson.at("range").get<float>();
    abilityTemplate.description = rootJson.at("description").get<std::string>();

    return abilityTemplate;
}

// Loads all ability files listed in a manifest.
std::vector<AbilityTemplate> AbilityLoader::loadAbilityTemplatesFromManifest(const std::string& manifestFilePath, const std::string& abilitiesDirectory) {
    std::ifstream inputFile(manifestFilePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open ability manifest file: " + manifestFilePath);
    }

    json manifestJson;

    try {
        inputFile >> manifestJson;
    } catch (const json::parse_error& error) {
        throw std::runtime_error("Failed to parse JSON in ability manifest file '" + manifestFilePath + "': " + error.what());
    }

    if (!manifestJson.is_array()) {
        throw std::runtime_error("Ability manifest file must contain a JSON array: " + manifestFilePath);
    }

    if (manifestJson.empty()) {
        throw std::runtime_error("Ability manifest file cannot be empty: " + manifestFilePath);
    }

    std::vector<AbilityTemplate> abilityTemplates;
    std::unordered_set<std::string> seenFileNames;

    for (std::size_t index = 0; index < manifestJson.size(); ++index) {
        const json& entry = manifestJson.at(index);

        if (!entry.is_string()) {
            throw std::runtime_error("Ability manifest entry at index " + std::to_string(index) + " must be a string file name.");
        }

        const std::string fileName = entry.get<std::string>();

        if (!seenFileNames.insert(fileName).second) {
            throw std::runtime_error("Duplicate file name found in ability manifest: " + fileName);
        }

        const std::string filePath = abilitiesDirectory + "/" + fileName;
        abilityTemplates.push_back(loadAbilityTemplateFromFile(filePath));
    }

    validateLoadedAbilityTemplates(abilityTemplates);

    return abilityTemplates;
}


