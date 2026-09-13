#pragma once

#include "cwsdk.h"
#include "core/Constants.h"
#include "core/MathUtils.h"

// Forward compatibility defines
#define LEVELS_PER_REGION xp_progression::kLevelsPerRegion
#define LEVEL_EQUIPMENT_CAP xp_progression::kLevelEquipmentCap

[[nodiscard]] int GetRegionDistance(IntVector2 region);
[[nodiscard]] int GetItemLevel(cube::Item* item);
[[nodiscard]] int GetCreatureLevel(cube::Creature* creature);
[[nodiscard]] int GetLevelVariation(long long modifier, int range);
void SetEquipmentRegion(cube::Creature* creature, IntVector2 region);

[[nodiscard]] unsigned long long ProgressionRand(unsigned long long seed);