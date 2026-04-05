#pragma once

#include <string>
#include <vector>

#include "data/UnitTemplate.h"

// Loads unit templates from JSON content files.
class UnitLoader {
    public:
    // Reads a single unit JSON file from disk and returns the resulting template.
    static UnitTemplate loadUnitTemplateFromFile(const std::string& filePath);

    // Reads a manifest file and loads all unit templates listed inside it.
    static std::vector<UnitTemplate> loadUnitTemplatesFromManifest(const std::string& manifestFilePath, const std::string& unitsDirectory);
};