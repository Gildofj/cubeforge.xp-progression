#pragma once

#include "../main.h"
#include "features/scaling/ScalingSystem.h"

// Global scaling system instance shared across hooks
inline pyro::ScalingSystem g_ScalingSystem;

extern "C" inline float GetGearScaling(cube::Item* item, cube::Creature* creature, int base)
{
    return g_ScalingSystem.CalculateGearScaling(item, creature, base);
}

extern "C" inline float GetHasteRe(cube::Item* item, cube::Creature* creature)
{
    return g_ScalingSystem.CalculateHaste(item, creature);
}

extern "C" inline float GetRegenRe(cube::Item* item, cube::Creature* creature)
{
    return g_ScalingSystem.CalculateRegen(item, creature);
}

extern "C" inline float GetCritRe(cube::Item* item, cube::Creature* creature)
{
    return g_ScalingSystem.CalculateCrit(item, creature);
}

inline void Setup_GearScalingOverwrite() {
#if defined(__GNUC__) || defined(__clang__)
    WriteFarJMP(CWOffset(0x109C50), reinterpret_cast<void*>(GetGearScaling));
    WriteFarJMP(CWOffset(0x10A490), reinterpret_cast<void*>(GetHasteRe));
    WriteFarJMP(CWOffset(0x109F30), reinterpret_cast<void*>(GetRegenRe));
    WriteFarJMP(CWOffset(0x1090F0), reinterpret_cast<void*>(GetCritRe));
#endif
}