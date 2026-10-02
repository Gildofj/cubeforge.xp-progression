#pragma once

#include "main.h"
#include "utility.h"
#include "features/hud/HUDFormatter.h"

extern "C" void RegionTextDrawOverwrite(plasma::Node* node, std::wstring* string)
{
    if (!node || !string) return;

    cube::Game* game = cube::GetGame();
    if (!game) return;

    cube::Creature* player = game->GetPlayer();
    if (!player) return;

    const int distance = GetRegionDistance(player->entity_data.current_region);
    const std::wstring prefix = xp_progression::HUDFormatter::FormatRegionBracket(distance);

    std::wstring fullString = prefix + *string;
    node->SetText(&fullString);
}

GETTER_VAR(void*, ASM_RegionTextDrawOverwrite_jmpback);
GETTER_VAR(void*, ASM_RegionTextDrawOverwrite_bail);

extern "C" void ASM_RegionTextDrawOverwrite();

inline void Setup_RegionTextDrawOverwrite() {
    WriteFarJMP(CWOffset(0xABA58), reinterpret_cast<void*>(&ASM_RegionTextDrawOverwrite));
    ASM_RegionTextDrawOverwrite_jmpback = reinterpret_cast<void*>(CWOffset(0xABA68));
    ASM_RegionTextDrawOverwrite_bail = reinterpret_cast<void*>(CWOffset(0xABA8C));
}