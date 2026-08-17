#pragma once

#include "cwsdk.h"
#include "../../core/StatTypes.h"
#include "../../core/MathUtils.h"

namespace pyro {

    class ScalingSystem {
    private:
        StatScalingTable m_playerScaling;
        StatScalingTable m_creatureScaling;

    public:
        ScalingSystem() noexcept;

        [[nodiscard]] const StatScalingTable& GetPlayerScaling() const noexcept { return m_playerScaling; }
        [[nodiscard]] const StatScalingTable& GetCreatureScaling() const noexcept { return m_creatureScaling; }

        void SetPlayerScaling(StatType type, float value) noexcept {
            m_playerScaling[ToIndex(type)] = value;
        }

        void SetCreatureScaling(StatType type, float value) noexcept {
            m_creatureScaling[ToIndex(type)] = value;
        }

        [[nodiscard]] float CalculateGearScaling(cube::Item* item, cube::Creature* creature, int base) const;
        [[nodiscard]] float CalculateOtherStats(cube::Item* item, cube::Creature* creature) const;
        [[nodiscard]] float CalculateHaste(cube::Item* item, cube::Creature* creature) const;
        [[nodiscard]] float CalculateRegen(cube::Item* item, cube::Creature* creature) const;
        [[nodiscard]] float CalculateCrit(cube::Item* item, cube::Creature* creature) const;

        void ApplyPlayerStatBuff(cube::Creature* creature, float* stat, StatType type, cube::Game* game) const;
        void ApplyCreatureStatBuff(cube::Creature* creature, float* stat, StatType type) const;
    };

} // namespace pyro
