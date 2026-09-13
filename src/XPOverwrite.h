#pragma once

#include "main.h"
#include "core/MathUtils.h"

extern "C" int XP_Overwrite(int level)
{
    return xp_progression::CalculateXPForLevel(level);
}

extern "C" void ASM_XP_Overwrite();

inline void Setup_XP_Overwrite() {
    WriteFarJMP(CWOffset(0x5FA80), reinterpret_cast<void*>(&ASM_XP_Overwrite));
}