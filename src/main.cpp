#include "main.h"
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

#include "core/Constants.h"
#include "core/StatTypes.h"
#include "core/MathUtils.h"
#include "utility.h"

#include "features/hud/HUDFormatter.h"
#include "features/scaling/ScalingSystem.h"
#include "features/progression/ProgressionSystem.h"
#include "features/network/NetworkSync.h"
#include "features/drops/DropSystem.h"

#include "XPOverwrite.h"
#include "LevelDisplayOverwrite.h"
#include "GearScalingOverWrite.h"
#include "GoldDropOverWrite.h"
#include "RegionTextDrawOverwrite.h"

/* Mod class containing all lifecycle callbacks for XP Progression.
 */
class Mod : public GenericMod {
private:
    void GainXP(cube::Game* game, int xp)
    {
        if (!game || xp <= 0) return;
        cube::Creature* player = game->GetPlayer();
        if (!player) return;

        FloatRGBA purple(xp_progression::kColorPurpleR, xp_progression::kColorPurpleG, xp_progression::kColorPurpleB, xp_progression::kColorPurpleA);
        wchar_t buffer[64];
        swprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), L"You gain %d xp.\n", xp);
        game->PrintMessage(buffer, &purple);

        player->entity_data.XP += xp;
    }

public:
    /* Hook for the chat function. Triggers when a user sends something in the chat.
     * @param    {std::wstring*} message
     * @return    {int} 1 if handled, 0 otherwise
    */
    virtual int OnChat(std::wstring* message) override {
        if (!message) return 0;

        const wchar_t* str = message->c_str();
        if (wcscmp(str, L"/recenter") == 0)
        {
            cube::Game* game = cube::GetGame();
            if (!game) return 1;

            cube::Creature* player = game->GetPlayer();
            if (!player)
            {
                game->PrintMessage(L"[Error] No local player found!\n", 255, 0, 0);
                return 1;
            }

            if (!xp_progression::NetworkSync::IsHost(game))
            {
                game->PrintMessage(L"[Error] Only the host can recenter the world progression origin.\n", 255, 0, 0);
                return 1;
            }

            // Set new center region
            player->entity_data.equipment.unk_item.region = player->entity_data.current_region;
            player->entity_data.equipment.unk_item.modifier = 1;

            xp_progression::NetworkSync::BroadcastHostBaseRegion(game, player->entity_data.current_region);

            if (game->world)
            {
                for (cube::Creature* creature : game->world->creatures)
                {
                    if (!creature) continue;
                    if (creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player)
                    {
                        // Ensure this command is not misused as a full heal
                        creature->entity_data.HP = std::min<float>(creature->GetMaxHP(), creature->entity_data.HP);
                    }
                    else
                    {
                        creature->entity_data.HP = creature->GetMaxHP();
                    }
                }
            }

            // Display success message
            game->PrintMessage(L"[Notification] Correctly recentered player region.\n", 0, 255, 0);
            return 1;
        }
        return 0;
    }

    virtual void OnGameUpdate(cube::Game* game) override {
        (void)game;
    }

    /* Function hook that gets called every game tick.
     * @param    {cube::Game*} game
     * @return    {void}
    */
    virtual void OnGameTick(cube::Game* game) override {
        if (!game) return;

        static bool init = false;
        if (!init)
        {
            init = true;

            // Disable old leveling system
            for (int i = 0; i < 10; i++)
            {
                WriteByte(CWOffset(0x6688F + i), 0x90);
            }
            for (int i = 0; i < 6; i++)
            {
                WriteByte(CWOffset(0x668FA + i), 0x90);
            }
        }

        cube::Creature* player = game->GetPlayer();
        if (!player)
        {
            return;
        }

        cube::Creature::EntityData* entity_data = &player->entity_data;

        // Show and move xp bar and level info
        if (game->gui.levelinfo_node)
        {
            plasma::Node* node = game->gui.levelinfo_node;
            node->SetVisibility(true);
            node->Translate(static_cast<float>(game->width / 2), static_cast<float>(game->height), -200.0f, -100.0f);
        }

        // Handle levelups
        while (entity_data->XP >= player->GetXPForLevelup())
        {
            entity_data->XP -= player->GetXPForLevelup();
            entity_data->level += 1;

            xp_progression::ProgressionSystem::ExecuteLevelUp(game, player);
        }

        // Set starting region if not set (Host/Singleplayer only)
        if (xp_progression::NetworkSync::IsHost(game))
        {
            cube::Item* storage = &entity_data->equipment.unk_item;
            if (storage->modifier == 0)
            {
                storage->modifier = 1;
                storage->region = entity_data->current_region;
            }
        }

        // Poll Steam P2P packets and update network state
        xp_progression::NetworkSync::PollIncomingPackets(game);
        xp_progression::NetworkSync::UpdateNetwork(game);
    }

    // Called for the host only
    virtual void OnCreatureDeath(cube::Game* game, cube::Creature* creature, cube::Creature* attacker) override {
        if (!game || !creature)
        {
            return;
        }

        if (!attacker)
        {
            attacker = game->GetPlayer();
        }
        if (!attacker)
        {
            return;
        }

        // Set correct position for items to drop
        if (creature->entity_data.current_region == IntVector2(0, 0))
        {
            creature->entity_data.some_position = creature->entity_data.position;
        }

        // Skip on mage bubbles
        if (creature->entity_data.race == 285)
        {
            return;
        }

        // Do not award XP when a player dies
        if (creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player)
        {
            return;
        }

        if (attacker->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player ||
            attacker->entity_data.hostility_type == cube::Creature::EntityBehaviour::Pet)
        {
            const float xp_gain = xp_progression::ProgressionSystem::CalculateCreatureKillXP(creature);
            cube::Creature* player = game->GetPlayer();
            if (player && xp_gain > 0.0f)
            {
                size_t connectionCount = 0;
                if (cube::SteamNetworking() && cube::SteamUser())
                {
                    const CSteamID mySteamID = cube::SteamUser()->GetSteamID();
                    for (const auto& conn : game->host.connections)
                    {
                        if (conn.first != mySteamID)
                        {
                            connectionCount++;
                        }
                    }
                }
                const int localXP = (connectionCount > 0)
                    ? static_cast<int>(xp_gain / static_cast<float>(connectionCount + 1))
                    : static_cast<int>(xp_gain);

                this->GainXP(game, localXP);
                xp_progression::NetworkSync::BroadcastXP(game, xp_gain);
            }
        }
    }

    virtual void OnGetItemBuyingPrice(cube::Item* item, int* price) override {
        xp_progression::DropSystem::AdjustItemBuyingPrice(item, price);
    }

    virtual void OnCreatureCanEquipItem(cube::Creature* creature, cube::Item* item, bool* equipable) override
    {
        if (!equipable) return;
        if (!xp_progression::ProgressionSystem::CanEquipItem(creature, item))
        {
            *equipable = false;
        }
    }

    virtual void OnClassCanWearItem(cube::Item* item, int classType, bool* wearable) override
    {
        cube::Game* game = cube::GetGame();
        if (!game) return;

        this->OnCreatureCanEquipItem(game->GetPlayer(), item, wearable);
    }

    /* Function hook that gets called on initialization of cubeworld.
     * [Note]: cube::GetGame() is not yet available here!!
     * @return {void}
    */
    virtual void Initialize() override {
        Setup_OverwriteGoldDrops();
        Setup_XP_Overwrite();
        Setup_LevelDisplayOverwrite();
        Setup_GearScalingOverwrite();
        Setup_RegionTextDrawOverwrite();
    }

    virtual void OnItemGetGoldBagValue(cube::Item* item, int* gold) override {
        if (!gold) return;
        cube::Game* game = cube::GetGame();
        if (!game) return;

        cube::Creature* player = game->GetPlayer();
        if (!player) return;

        *gold = xp_progression::DropSystem::CalculateGoldBagValue(GetRegionDistance(player->entity_data.current_region));
    }

    virtual void OnCreatureArmorCalculated(cube::Creature* creature, float* armor) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, armor, xp_progression::StatType::ARMOR, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, armor, xp_progression::StatType::ARMOR);
    }

    virtual void OnCreatureCriticalCalculated(cube::Creature* creature, float* critical) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, critical, xp_progression::StatType::CRIT, cube::GetGame());
    }

    virtual void OnCreatureAttackPowerCalculated(cube::Creature* creature, float* power) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, power, xp_progression::StatType::ATK_POWER, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, power, xp_progression::StatType::ATK_POWER);
    }

    virtual void OnCreatureSpellPowerCalculated(cube::Creature* creature, float* power) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, power, xp_progression::StatType::SPELL_POWER, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, power, xp_progression::StatType::SPELL_POWER);
    }

    virtual void OnCreatureHasteCalculated(cube::Creature* creature, float* haste) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, haste, xp_progression::StatType::HASTE, cube::GetGame());
    }

    virtual void OnCreatureHPCalculated(cube::Creature* creature, float* hp) override {
        if (creature && creature->entity_data.hostility_type != cube::Creature::EntityBehaviour::Player)
        {
            SetEquipmentRegion(creature, creature->entity_data.current_region);
        }

        g_ScalingSystem.ApplyPlayerStatBuff(creature, hp, xp_progression::StatType::HEALTH, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, hp, xp_progression::StatType::HEALTH);
    }

    virtual void OnCreatureResistanceCalculated(cube::Creature* creature, float* resistance) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, resistance, xp_progression::StatType::RESISTANCE, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, resistance, xp_progression::StatType::RESISTANCE);
    }

    virtual void OnCreatureRegenerationCalculated(cube::Creature* creature, float* regeneration) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, regeneration, xp_progression::StatType::STAMINA, cube::GetGame());
    }

    virtual void OnCreatureManaGenerationCalculated(cube::Creature* creature, float* manaGeneration) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, manaGeneration, xp_progression::StatType::MANA, cube::GetGame());
    }
};

// Export of the mod created in this file, so that the modloader can see and use it.
EXPORT Mod* MakeMod() {
    return new Mod();
}
