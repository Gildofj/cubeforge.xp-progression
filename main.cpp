#include "main.h"
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

#include "src/core/Constants.h"
#include "src/core/StatTypes.h"
#include "src/core/MathUtils.h"
#include "src/utility.h"

#include "src/features/hud/HUDFormatter.h"
#include "src/features/scaling/ScalingSystem.h"
#include "src/features/progression/ProgressionSystem.h"
#include "src/features/network/NetworkSync.h"
#include "src/features/drops/DropSystem.h"

#include "src/XPOverwrite.h"
#include "src/LevelDisplayOverwrite.h"
#include "src/GearScalingOverWrite.h"
#include "src/GoldDropOverWrite.h"
#include "src/RegionTextDrawOverwrite.h"

/* Mod class containing all lifecycle callbacks for PyroProgression.
 */
class Mod : public GenericMod {
private:
    std::deque<cube::TextFX> m_FXList;

    void GainXP(cube::Game* game, int xp)
    {
        if (!game) return;
        cube::Creature* player = game->GetPlayer();
        if (!player) return;

        FloatRGBA purple(pyro::kColorPurpleR, pyro::kColorPurpleG, pyro::kColorPurpleB, pyro::kColorPurpleA);
        wchar_t buffer[64];
        swprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), L"You gain %d xp.\n", xp);
        game->PrintMessage(buffer, &purple);

        cube::TextFX xpText = cube::TextFX();
        xpText.position = player->entity_data.position + LongVector3(
            std::rand() % (cube::DOTS_PER_BLOCK * 50),
            std::rand() % (cube::DOTS_PER_BLOCK * 50),
            std::rand() % (cube::DOTS_PER_BLOCK * 50)
        );
        xpText.animation_length = pyro::kTextFXXPGainAnimLength;
        xpText.distance_to_fall = pyro::kTextFXXPGainDistance;
        xpText.color = purple;
        xpText.size = pyro::kTextFXXPGainSize;
        xpText.offset_2d = FloatVector2(-50.0f, -100.0f);
        xpText.text = std::wstring(L"+") + std::to_wstring(xp) + std::wstring(L" XP");
        xpText.field_60 = 0;

        for (int i = 0; i < 4; ++i)
        {
            m_FXList.push_back(xpText);
        }

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

            // Set new center region
            player->entity_data.equipment.unk_item.region = player->entity_data.current_region;

            // Reset HP of all creatures
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
        if (!game) return;
        if (!m_FXList.empty())
        {
            game->textfx_list.push_back(m_FXList.front());
            m_FXList.pop_front();
        }
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
            node->Translate(game->width / 2, game->height, -200, -100);
        }

        // Handle levelups
        while (entity_data->XP >= player->GetXPForLevelup())
        {
            entity_data->XP -= player->GetXPForLevelup();
            entity_data->level += 1;

            pyro::ProgressionSystem::ExecuteLevelUp(game, player);
        }

        // Set starting region if not set
        cube::Item* storage = &entity_data->equipment.unk_item;
        if (storage->modifier == 0)
        {
            storage->modifier = 1;
            storage->region = entity_data->current_region;

            constexpr int modifier = 0;
            entity_data->equipment.weapon_right.modifier = modifier;
            entity_data->equipment.weapon_left.modifier = modifier;
            entity_data->equipment.chest.modifier = modifier;
            entity_data->equipment.feet.modifier = modifier;
            entity_data->equipment.hands.modifier = modifier;
            entity_data->equipment.neck.modifier = modifier;
            entity_data->equipment.shoulder.modifier = modifier;
            entity_data->equipment.ring_left.modifier = modifier;
            entity_data->equipment.ring_right.modifier = modifier;

            if (player->inventory_tabs.size() > 1)
            {
                for (cube::ItemStack& itemstack : player->inventory_tabs.at(0))
                {
                    itemstack.item.modifier = modifier;
                }
            }
        }

        // Poll Steam P2P packets
        pyro::NetworkSync::PollIncomingPackets(game);
    }

    // Called for the host only
    virtual void OnCreatureDeath(cube::Game* game, cube::Creature* creature, cube::Creature* attacker) override {
        if (!game || !creature || !attacker)
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

        if (attacker->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player ||
            attacker->entity_data.hostility_type == cube::Creature::EntityBehaviour::Pet)
        {
            const float xp_gain = pyro::ProgressionSystem::CalculateCreatureKillXP(creature);
            pyro::NetworkSync::BroadcastXP(game, xp_gain);
        }
    }

    virtual void OnGetItemBuyingPrice(cube::Item* item, int* price) override {
        pyro::DropSystem::AdjustItemBuyingPrice(item, price);
    }

    virtual void OnCreatureCanEquipItem(cube::Creature* creature, cube::Item* item, bool* equipable) override
    {
        if (!equipable) return;
        if (!pyro::ProgressionSystem::CanEquipItem(creature, item))
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

        *gold = pyro::DropSystem::CalculateGoldBagValue(GetRegionDistance(player->entity_data.current_region));
    }

    virtual void OnCreatureArmorCalculated(cube::Creature* creature, float* armor) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, armor, pyro::StatType::ARMOR, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, armor, pyro::StatType::ARMOR);
    }

    virtual void OnCreatureCriticalCalculated(cube::Creature* creature, float* critical) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, critical, pyro::StatType::CRIT, cube::GetGame());
    }

    virtual void OnCreatureAttackPowerCalculated(cube::Creature* creature, float* power) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, power, pyro::StatType::ATK_POWER, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, power, pyro::StatType::ATK_POWER);
    }

    virtual void OnCreatureSpellPowerCalculated(cube::Creature* creature, float* power) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, power, pyro::StatType::SPELL_POWER, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, power, pyro::StatType::SPELL_POWER);
    }

    virtual void OnCreatureHasteCalculated(cube::Creature* creature, float* haste) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, haste, pyro::StatType::HASTE, cube::GetGame());
    }

    virtual void OnCreatureHPCalculated(cube::Creature* creature, float* hp) override {
        if (creature && creature->entity_data.hostility_type != cube::Creature::EntityBehaviour::Player)
        {
            SetEquipmentRegion(creature, creature->entity_data.current_region);
        }

        g_ScalingSystem.ApplyPlayerStatBuff(creature, hp, pyro::StatType::HEALTH, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, hp, pyro::StatType::HEALTH);
    }

    virtual void OnCreatureResistanceCalculated(cube::Creature* creature, float* resistance) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, resistance, pyro::StatType::RESISTANCE, cube::GetGame());
        g_ScalingSystem.ApplyCreatureStatBuff(creature, resistance, pyro::StatType::RESISTANCE);
    }

    virtual void OnCreatureRegenerationCalculated(cube::Creature* creature, float* regeneration) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, regeneration, pyro::StatType::STAMINA, cube::GetGame());
    }

    virtual void OnCreatureManaGenerationCalculated(cube::Creature* creature, float* manaGeneration) override {
        g_ScalingSystem.ApplyPlayerStatBuff(creature, manaGeneration, pyro::StatType::MANA, cube::GetGame());
    }
};

// Export of the mod created in this file, so that the modloader can see and use it.
EXPORT Mod* MakeMod() {
    return new Mod();
}