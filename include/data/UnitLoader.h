#pragma once

#include <string>
#include <vector>

#include "data/UnitTemplate.h"

// Loads unit templates from a JSON file.
class UnitLoader {
    public:
    // Reads a JSON file from disk and returns all unit templates found within it.
    static std::vector<UnitTemplate> loadUnitTemplatesFromFile(const std::string& filePath);
};