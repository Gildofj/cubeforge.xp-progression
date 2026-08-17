#pragma once

#include "../main.h"
#include "utility.h"
#include "features/hud/HUDFormatter.h"

inline void PutText(void* unk, wchar_t* buffer)
{
    reinterpret_cast<void (*)(void*, wchar_t*)>(CWOffset(0x486B0))(unk, buffer);
}

extern "C" inline void OverwriteItemName(cube::Item* item, std::wstring* string)
{
    if (!item || !string || item->category < 3 || item->category > 9)
    {
        return;
    }

    const double item_level = static_cast<double>(GetItemLevel(item));
    const std::wstring prefix = pyro::HUDFormatter::FormatLevelPrefix(item_level);
    *string = prefix + *string;
}

extern "C" inline void LevelDisplayOverwriteCreature(cube::Creature* creature, void* unk)
{
    if (!creature || !unk) return;

    const double creature_level = static_cast<double>(GetCreatureLevel(creature));
    const std::wstring prefix = pyro::HUDFormatter::FormatLevelPrefix(creature_level);

    wchar_t buffer[64];
    wcsncpy_s(buffer, prefix.c_str(), _TRUNCATE);
    PutText(unk, buffer);
}

extern "C" inline void sub_336F0(void* a1, int a2, int a3)
{
    reinterpret_cast<void (*)(void*, int, int)>(CWOffset(0x336F0))(a1, a2, a3);
}

GETTER_VAR(void*, ASM_LevelDisplayOverwrite_jmpback);

#if defined(__GNUC__) || defined(__clang__)
NAKED_FN void ASM_LevelDisplayOverwrite() {
    asm(".intel_syntax \n"
        "xor r8d, r8d \n"
        "mov dl, 0x1 \n"
        "lea rcx, [rbp + 0x78] \n"
        "call sub_336F0 \n"

        "lea rdx, [rbp - 0x80] \n"
        "mov rcx, [r14] \n"
        "call LevelDisplayOverwriteCreature \n"

        DEREF_JMP(ASM_LevelDisplayOverwrite_jmpback)
    );
}

NAKED_FN void ASM_OverwriteItemName() {
    asm(".intel_syntax \n"
        // rdi: item
        // rsi: wstring
        "mov rdx, rsi \n"
        "lea rcx, [rbp + 0x60] \n"
        "call OverwriteItemName \n"

        // Old code
        "mov rax, rsi \n"
        "mov rcx, [rbp + 0x280] \n"
        "xor rcx, rsp \n"
        "mov rbx, [rsp + 0x3C8] \n"
        "add rsp, 0x390 \n"
        "pop rdi \n"
        "pop rsi \n"
        "pop rbp \n"
        "retn \n"
    );
}
#else
inline void ASM_LevelDisplayOverwrite() {}
inline void ASM_OverwriteItemName() {}
#endif

inline void Setup_LevelDisplayOverwrite() {
#if defined(__GNUC__) || defined(__clang__)
    WriteFarJMP(CWOffset(0xB1966), reinterpret_cast<void*>(&ASM_LevelDisplayOverwrite));
    ASM_LevelDisplayOverwrite_jmpback = reinterpret_cast<void*>(CWOffset(0xB19AE));

    WriteFarJMP(CWOffset(0x16466C), reinterpret_cast<void*>(&ASM_OverwriteItemName));
#endif
}