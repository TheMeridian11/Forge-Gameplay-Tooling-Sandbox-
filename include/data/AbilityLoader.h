#pragma once

#include <string>
#include <vector>

#include "data/AbilityTemplate.h"

class AbilityLoader {
    public:
    // Reads a single ability JSON file from disk and returns the resulting template.
    static AbilityTemplate loadAbilityTemplateFromFile(const std::string& filePath);

    // Reads a manifest file and loads all ability templates listed inside it.
    static std::vector<AbilityTemplate> loadAbilityTemplatesFromManifest(const std::string& manifestFilePath, const std::string& abilitiesDirectory);
};