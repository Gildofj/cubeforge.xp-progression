#pragma once

#include "main.h"
#include "features/drops/DropSystem.h"

extern "C" void GetGoldDrops(cube::Creature* creature, float* gold)
{
    xp_progression::DropSystem::ProcessGoldDrops(creature, gold);
}

GETTER_VAR(void*, ASM_OverwriteGoldDrops_jmpback);

extern "C" void ASM_OverwriteGoldDrops();

inline void Setup_OverwriteGoldDrops() {
    WriteFarJMP(CWOffset(0x2A752C), reinterpret_cast<void*>(&ASM_OverwriteGoldDrops));
    ASM_OverwriteGoldDrops_jmpback = reinterpret_cast<void*>(CWOffset(0x2A7540));
}