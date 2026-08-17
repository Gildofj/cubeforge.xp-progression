#pragma once

#include "../main.h"
#include "utility.h"
#include "features/hud/HUDFormatter.h"

extern "C" inline void RegionTextDrawOverwrite(plasma::Node* node, std::wstring* string)
{
    if (!node || !string) return;

    cube::Game* game = cube::GetGame();
    if (!game) return;

    cube::Creature* player = game->GetPlayer();
    if (!player) return;

    const int distance = GetRegionDistance(player->entity_data.current_region);
    const std::wstring prefix = pyro::HUDFormatter::FormatRegionBracket(distance);

    *string = prefix + *string;
    node->SetText(string);
}

GETTER_VAR(void*, ASM_RegionTextDrawOverwrite_jmpback);
GETTER_VAR(void*, ASM_RegionTextDrawOverwrite_bail);

#if defined(__GNUC__) || defined(__clang__)
NAKED_FN void ASM_RegionTextDrawOverwrite() {
    asm(".intel_syntax \n"
        
        // String is already set in rdx
        "mov rcx, [r13 + 0x2D0] \n"
        "call RegionTextDrawOverwrite \n"

        "test esi, esi \n"
        "jz 1f \n"

        DEREF_JMP(ASM_RegionTextDrawOverwrite_jmpback)

        "1: \n"

        DEREF_JMP(ASM_RegionTextDrawOverwrite_bail)
    );
}
#else
inline void ASM_RegionTextDrawOverwrite() {}
#endif

inline void Setup_RegionTextDrawOverwrite() {
#if defined(__GNUC__) || defined(__clang__)
    WriteFarJMP(CWOffset(0xABA58), reinterpret_cast<void*>(&ASM_RegionTextDrawOverwrite));
    ASM_RegionTextDrawOverwrite_jmpback = reinterpret_cast<void*>(CWOffset(0xABA68));
    ASM_RegionTextDrawOverwrite_bail = reinterpret_cast<void*>(CWOffset(0xABA8C));
#endif
}