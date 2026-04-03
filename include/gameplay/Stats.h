#pragma once

// Stores the core numerical gameplay values for a unit.
// This will expand later as the sandbox becomes more advanced
struct Stats {
    int max_health = 0;
    int current_health = 0;
    int attack_power = 0;

    // Tracks fractional regeneration progress so that we can support smooth health
    // regeneration over time even though health itself is stored as an integer.
    // NOTE: The reason for this variable existing is because if we regenerate, for
    // example 5 health per second, and our frame time is something like "0.016", then
    // each frame only adds a fraction of health.
    // Example: 5.0 * 0.016 = 0.08
    // ^^but health is stored as an int, so we cannot add 0.08 directly to "currentHealth".
    // so instead, my plan is to:
    // 1) accumulate fractional regeneration in "regeneration_progress"
    // 2) once it reaches at least 1.0, we conver that into actual health points.
    float regeneration_progress = 0.0f;
};