#pragma once

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include "cwsdk.h"
#include "Constants.h"

namespace pyro {

    /**
     * @brief Deterministic Linear Congruential Generator (LCG).
     * Matches Cube World / Pyro mod progression pseudorandom sequence.
     */
    [[nodiscard]] constexpr uint64_t PyroRand(uint64_t seed) noexcept {
        const uint64_t n = seed * 1103515245ULL + 12345ULL;
        return (n / 65536ULL) % 32768ULL;
    }

    /**
     * @brief Computes level variation for a given modifier within a range.
     */
    [[nodiscard]] constexpr int GetLevelVariation(int64_t modifier, int range) noexcept {
        if (range <= 0) return 0;
        return static_cast<int>(PyroRand(static_cast<uint64_t>(modifier)) % static_cast<uint64_t>(range));
    }

    /**
     * @brief Computes Chebyshev distance (L-infinity norm) between two 2D grid coordinates.
     */
    [[nodiscard]] inline int CalculateChebyshevDistance(IntVector2 from, IntVector2 to) noexcept {
        const int xDiv = std::abs(from.x - to.x);
        const int yDiv = std::abs(from.y - to.y);
        return (xDiv > yDiv) ? xDiv : yDiv;
    }

    /**
     * @brief Calculates item level based on distance from base region and item modifier.
     */
    [[nodiscard]] constexpr int CalculateItemLevel(int regionDistance, int64_t modifier, int rarity = 1) noexcept {
        return 1 + (kLevelsPerRegion * regionDistance) + GetLevelVariation(modifier, kLevelsPerRegion);
    }

    /**
     * @brief Calculates creature level based on distance from base region and creature id/modifier.
     */
    [[nodiscard]] constexpr int CalculateCreatureLevel(int regionDistance, int64_t creatureId) noexcept {
        return 1 + (kLevelsPerRegion * regionDistance) + GetLevelVariation(creatureId, kLevelsPerRegion);
    }

    /**
     * @brief Calculates required XP for next level using exponential power curve.
     * Formula: 50 * (1 + level^1.3)
     */
    [[nodiscard]] inline int CalculateXPForLevel(int level) noexcept {
        if (level < 1) level = 1;
        return static_cast<int>(50.0f * (1.0f + std::pow(static_cast<float>(level), 1.3f)));
    }

} // namespace pyro
