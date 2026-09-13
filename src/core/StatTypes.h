#pragma once

#include <cstdint>
#include <array>
#include <cstddef>

namespace xp_progression {

    enum class StatType : uint8_t {
        ARMOR = 0,
        CRIT,
        ATK_POWER,
        SPELL_POWER,
        HASTE,
        HEALTH,
        RESISTANCE,
        STAMINA,
        MANA,
        COUNT
    };

    constexpr size_t kStatCount = static_cast<size_t>(StatType::COUNT);

    using StatScalingTable = std::array<float, kStatCount>;

    [[nodiscard]] constexpr size_t ToIndex(StatType type) noexcept {
        return static_cast<size_t>(type);
    }

    [[nodiscard]] inline StatScalingTable CreateDefaultPlayerScaling() noexcept {
        StatScalingTable table{};
        table[ToIndex(StatType::HEALTH)]      = 1.0f;
        table[ToIndex(StatType::ARMOR)]       = 0.2f;
        table[ToIndex(StatType::RESISTANCE)]  = 0.2f;
        table[ToIndex(StatType::ATK_POWER)]   = 0.2f;
        table[ToIndex(StatType::SPELL_POWER)] = 0.2f;
        table[ToIndex(StatType::CRIT)]        = 0.0001f;
        table[ToIndex(StatType::HASTE)]       = 0.0001f;
        table[ToIndex(StatType::STAMINA)]     = 0.05f;
        table[ToIndex(StatType::MANA)]        = 0.05f;
        return table;
    }

    [[nodiscard]] inline StatScalingTable CreateDefaultCreatureScaling() noexcept {
        StatScalingTable table{};
        table[ToIndex(StatType::HEALTH)]      = 2.0f;
        table[ToIndex(StatType::ARMOR)]       = 0.01f;
        table[ToIndex(StatType::RESISTANCE)]  = 0.01f;
        table[ToIndex(StatType::ATK_POWER)]   = 3.0f;
        table[ToIndex(StatType::SPELL_POWER)] = 3.0f;
        table[ToIndex(StatType::CRIT)]        = 0.0f;
        table[ToIndex(StatType::HASTE)]       = 0.0f;
        table[ToIndex(StatType::STAMINA)]     = 0.0f;
        table[ToIndex(StatType::MANA)]        = 0.0f;
        return table;
    }

} // namespace xp_progression
