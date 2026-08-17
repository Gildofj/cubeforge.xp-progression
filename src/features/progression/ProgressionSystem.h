#pragma once

#include "cwsdk.h"
#include "../../core/Constants.h"
#include "../../core/MathUtils.h"

namespace pyro {

    class ProgressionSystem {
    public:
        [[nodiscard]] static int GetXPForLevel(int level) noexcept {
            return CalculateXPForLevel(level);
        }

        [[nodiscard]] static bool CanEquipItem(const cube::Creature* creature, const cube::Item* item) noexcept;

        [[nodiscard]] static float CalculateCreatureKillXP(const cube::Creature* creature) noexcept;

        static void ExecuteLevelUp(cube::Game* game, cube::Creature* creature);
    };

} // namespace pyro
