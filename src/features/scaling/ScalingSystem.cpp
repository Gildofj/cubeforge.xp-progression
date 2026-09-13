#include "ScalingSystem.h"
#include "../../utility.h"
#include <cmath>

namespace xp_progression {

    ScalingSystem::ScalingSystem() noexcept
        : m_playerScaling(CreateDefaultPlayerScaling()),
          m_creatureScaling(CreateDefaultCreatureScaling())
    {
    }

    float ScalingSystem::CalculateGearScaling(cube::Item* item, cube::Creature* creature, int base) const {
        if (!item) return 0.0f;

        IntVector2 region;
        cube::Game* game = cube::GetGame();
        cube::Creature* player = game ? game->GetPlayer() : nullptr;

        if (!creature || creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player) {
            if (creature) {
                region = creature->entity_data.current_region;
            } else if (player) {
                region = player->entity_data.current_region;
            } else {
                region = item->region;
            }
        } else {
            region = item->region;
        }

        const int mod = item->modifier;
        const int mod1 = mod ^ (mod << 0x0D);
        const int mod2 = mod1 ^ (mod1 >> 0x11);
        const int mod3 = mod2 ^ (mod2 << 0x05);

        const int effective_rarity = item->GetEffectiveRarity(&region) + 1;
        const float base_res = ((base * 0.5f) / 32.0f);
        const float mod_modifier = (static_cast<float>(mod3) / static_cast<float>(0x10624DD3)) / 8.0f;

        constexpr float X = 0.5f;
        float result = std::abs(X + base_res);

        if (!creature || (creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player)) {
            const float itemLevel = static_cast<float>(GetItemLevel(item));
            result *= std::log2((itemLevel + 3.0f * (effective_rarity + mod_modifier) + 1001.0f) / 1000.0f) * 1000.0f;
        } else {
            result *= 3.0f * (effective_rarity + mod_modifier + 1.0f);
        }

        return result;
    }

    float ScalingSystem::CalculateOtherStats(cube::Item* item, cube::Creature* creature) const {
        if (!item) return 0.0f;

        IntVector2 region;
        cube::Game* game = cube::GetGame();
        cube::Creature* player = game ? game->GetPlayer() : nullptr;

        if (!creature || creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player) {
            if (creature) {
                region = creature->entity_data.current_region;
            } else if (player) {
                region = player->entity_data.current_region;
            } else {
                region = item->region;
            }
        } else {
            region = item->region;
        }

        const int mod = item->modifier;
        const int mod1 = mod ^ (mod << 0x0D);
        const int mod2 = mod1 ^ (mod1 >> 0x11);
        const int mod3 = mod2 ^ (mod2 << 0x05);

        const int effective_rarity = item->GetEffectiveRarity(&region) + 1;
        const float mod_modifier = (static_cast<float>(mod3) / static_cast<float>(0x10624DD3)) / 7.0f;

        constexpr float X = 1.2f;
        const float Y = effective_rarity + mod_modifier;

        float result = std::pow(X, Y);

        if (!creature || (creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player)) {
            result *= 0.01f + 0.0016f * static_cast<float>(GetItemLevel(item));
        }

        result *= 0.1f;
        return result;
    }

    float ScalingSystem::CalculateHaste(cube::Item* item, cube::Creature* creature) const {
        if (!item) return 0.0f;
        const int category = item->category;
        if (category < 3 || category > 9) {
            return 0.0f;
        }
        return CalculateOtherStats(item, creature) * (1.0f + static_cast<float>(ProgressionRand(item->modifier)) / 32768.0f);
    }

    float ScalingSystem::CalculateRegen(cube::Item* item, cube::Creature* creature) const {
        if (!item) return 0.0f;
        const int category = item->category;
        if ((category < 4 || category > 9) && category != 26) {
            return 0.0f;
        }
        return CalculateOtherStats(item, creature) * (1.0f + static_cast<float>(ProgressionRand(item->modifier + 0x157)) / 32768.0f);
    }

    float ScalingSystem::CalculateCrit(cube::Item* item, cube::Creature* creature) const {
        if (!item) return 0.0f;
        const int category = item->category;
        if (category < 3 || category > 10) {
            return 0.0f;
        }
        return CalculateOtherStats(item, creature) * (1.0f + static_cast<float>(ProgressionRand(item->modifier + 0x99)) / 32768.0f);
    }

    void ScalingSystem::ApplyPlayerStatBuff(cube::Creature* creature, float* stat, StatType type, cube::Game* game) const {
        if (!creature || !stat || !game) return;

        cube::Creature* player = game->GetPlayer();
        if (!player || creature->id != player->id) return;

        switch (type) {
        case StatType::HEALTH:
            *stat -= 50.0f;
            break;
        case StatType::ATK_POWER:
        case StatType::SPELL_POWER:
            *stat -= 2.0f;
            break;
        default:
            break;
        }

        *stat += m_playerScaling[ToIndex(type)] * static_cast<float>(creature->entity_data.level);
    }

    void ScalingSystem::ApplyCreatureStatBuff(cube::Creature* creature, float* stat, StatType type) const {
        if (!creature || !stat) return;

        if (creature->entity_data.hostility_type != cube::Creature::EntityBehaviour::Player &&
            creature->entity_data.hostility_type != cube::Creature::EntityBehaviour::Pet)
        {
            const float creatureLevel = static_cast<float>(GetCreatureLevel(creature));
            const float scalingFactor = m_creatureScaling[ToIndex(type)];
            *stat *= 0.05f * (1.0f + scalingFactor) * (1.0f + std::log2((creatureLevel + 1001.0f) / 1000.0f) * 1000.0f);
        }
    }

} // namespace xp_progression
