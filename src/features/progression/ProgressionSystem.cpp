#include "ProgressionSystem.h"
#include "../../utility.h"

namespace xp_progression {

    bool ProgressionSystem::CanEquipItem(const cube::Creature* creature, const cube::Item* item) noexcept {
        if (!creature || !item) return true;

        if (creature->entity_data.hostility_type != cube::Creature::EntityBehaviour::Player) {
            return true;
        }

        if (item->category < 3 || item->category > 9) {
            return true;
        }

        // Equipment cap check: player level + cap must be >= item level
        const int itemLevel = GetItemLevel(const_cast<cube::Item*>(item));
        return (creature->entity_data.level + kLevelEquipmentCap >= itemLevel);
    }

    float ProgressionSystem::CalculateCreatureKillXP(const cube::Creature* creature) noexcept {
        if (!creature) return 0.0f;

        const int level = GetCreatureLevel(const_cast<cube::Creature*>(creature));
        const int stars = creature->entity_data.level + 1;

        float xp_gain = static_cast<float>(level * stars);

        if ((creature->entity_data.appearance.flags2 & (1 << static_cast<int>(cube::Creature::AppearanceModifiers::IsBoss))) != 0) {
            xp_gain *= 10.0f;
        }

        if ((creature->entity_data.appearance.flags2 & (1 << static_cast<int>(cube::Creature::AppearanceModifiers::IsNamedBoss))) != 0) {
            xp_gain *= 5.0f;
        }

        if ((creature->entity_data.appearance.flags2 & (1 << static_cast<int>(cube::Creature::AppearanceModifiers::IsMiniBoss))) != 0) {
            xp_gain *= 2.0f;
        }

        return xp_gain;
    }

    void ProgressionSystem::ExecuteLevelUp(cube::Game* game, cube::Creature* creature) {
        if (!game || !creature) return;

        FloatRGBA purple(kColorPurpleR, kColorPurpleG, kColorPurpleB, kColorPurpleA);

        // Play levelup sound effect
        game->PlaySoundEffect(cube::Game::SoundEffect::sound_level_up);

        // Print levelup message to chat
        game->PrintMessage(L"LEVEL UP!\n", &purple);

        // Restore full health on level up
        creature->entity_data.HP = creature->GetMaxHP();
    }

} // namespace xp_progression
