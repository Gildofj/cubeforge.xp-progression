#include "DropSystem.h"
#include "../../utility.h"

namespace xp_progression {

    void DropSystem::SetGoldDropValue(int value) {
        constexpr uint64_t address = 0x2A7602;
        constexpr uint64_t offset = 0x04;

        WriteByte(CWOffset(address + offset), value & 0xFF);
        WriteByte(CWOffset(address + offset + 1), (value >> 8) & 0xFF);
        WriteByte(CWOffset(address + offset + 2), (value >> 16) & 0xFF);
        WriteByte(CWOffset(address + offset + 3), (value >> 24) & 0xFF);
    }

    void DropSystem::ProcessGoldDrops(cube::Creature* creature, float* gold) {
        if (!gold) return;

        if (creature) {
            SetGoldDropValue(GetCreatureLevel(creature));
        }

        if (*gold <= 0.0f) {
            *gold = 1.0f;
        }
    }

    int DropSystem::CalculateGoldBagValue(int regionDistance) noexcept {
        return 100 * (regionDistance + 1);
    }

    void DropSystem::AdjustItemBuyingPrice(cube::Item* item, int* price) {
        if (!item || !price) return;

        const int category = item->category;

        if (category >= 3 && category <= 9) {
            const int level = GetItemLevel(item);
            *price *= level;
            *price *= *price;
        } else if (category == 15) {
            *price *= *price;
            *price *= 2 * (1 + GetRegionDistance(item->region));
        }
    }

} // namespace xp_progression
