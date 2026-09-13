#pragma once

#include "cwsdk.h"

namespace xp_progression {

    class DropSystem {
    public:
        /**
         * @brief Patches gold drop operand in game memory for creature drop generation.
         */
        static void SetGoldDropValue(int value);

        /**
         * @brief Hook handler for creature gold drop calculation.
         */
        static void ProcessGoldDrops(cube::Creature* creature, float* gold);

        /**
         * @brief Calculates gold coin bag value based on region distance.
         */
        [[nodiscard]] static int CalculateGoldBagValue(int regionDistance) noexcept;

        /**
         * @brief Adjusts item buying price according to category and scaling.
         */
        static void AdjustItemBuyingPrice(cube::Item* item, int* price);
    };

} // namespace xp_progression
