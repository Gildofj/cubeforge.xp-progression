#pragma once

#include "main.h"
#include "utility.h"
#include "features/hud/HUDFormatter.h"

inline void PutText(void* unk, wchar_t* buffer)
{
    reinterpret_cast<void (*)(void*, wchar_t*)>(CWOffset(0x486B0))(unk, buffer);
}

extern "C" void OverwriteItemName(cube::Item* item, std::wstring* string)
{
    if (!item || !string || item->category < 3 || item->category > 9)
    {
        return;
    }

    const double item_level = static_cast<double>(GetItemLevel(item));
    const std::wstring prefix = xp_progression::HUDFormatter::FormatLevelPrefix(item_level);
    *string = prefix + *string;
}

extern "C" void LevelDisplayOverwriteCreature(cube::Creature* creature, void* unk)
{
    if (!creature || !unk) return;

    const double creature_level = static_cast<double>(GetCreatureLevel(creature));
    const std::wstring prefix = xp_progression::HUDFormatter::FormatLevelPrefix(creature_level);

    wchar_t buffer[64];
    wcsncpy_s(buffer, prefix.c_str(), _TRUNCATE);
    PutText(unk, buffer);
}

extern "C" void sub_336F0(void* a1, int a2, int a3)
{
    reinterpret_cast<void (*)(void*, int, int)>(CWOffset(0x336F0))(a1, a2, a3);
}

GETTER_VAR(void*, ASM_LevelDisplayOverwrite_jmpback);

extern "C" void ASM_LevelDisplayOverwrite();
extern "C" void ASM_OverwriteItemName();

inline void Setup_LevelDisplayOverwrite() {
    WriteFarJMP(CWOffset(0xB1966), reinterpret_cast<void*>(&ASM_LevelDisplayOverwrite));
    ASM_LevelDisplayOverwrite_jmpback = reinterpret_cast<void*>(CWOffset(0xB19AE));

    WriteFarJMP(CWOffset(0x16466C), reinterpret_cast<void*>(&ASM_OverwriteItemName));
}