#include "utility.h"
#include "features/network/NetworkSync.h"

IntVector2 GetWorldBaseRegion()
{
    cube::Game* game = cube::GetGame();
    if (!game)
    {
        return IntVector2(0, 0);
    }

    cube::Creature* player = game->GetPlayer();
    if (!player)
    {
        return IntVector2(0, 0);
    }

    // 1. If we are the Host (Singleplayer or Multiplayer Host)
    if (xp_progression::NetworkSync::IsHost(game))
    {
        IntVector2 base_region = player->entity_data.equipment.unk_item.region;
        if (player->entity_data.equipment.unk_item.modifier == 0 || 
            (base_region == IntVector2(0, 0) && player->entity_data.current_region != IntVector2(0, 0)))
        {
            base_region = player->entity_data.current_region;
        }
        return base_region;
    }

    // 2. We are a Client joining a Host.
    // The Host MUST ALWAYS have highest priority in multiplayer!

    // Priority 1: Synced Host Base Region from Steam P2P Network
    const auto syncedRegion = xp_progression::NetworkSync::GetSyncedHostBaseRegion();
    if (syncedRegion.has_value())
    {
        return *syncedRegion;
    }

    // Priority 2: Host Creature in the loaded world
    if (game->world && game->client.host_steam_id.IsValid())
    {
        const uint64_t hostSteamID = game->client.host_steam_id.ConvertToUint64();
        for (cube::Creature* creature : game->world->creatures)
        {
            if (!creature) continue;
            if (creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player &&
                static_cast<uint64_t>(creature->entity_data.steam_id) == hostSteamID)
            {
                const IntVector2 host_storage_region = creature->entity_data.equipment.unk_item.region;
                if (creature->entity_data.equipment.unk_item.modifier != 0 && host_storage_region != IntVector2(0, 0))
                {
                    return host_storage_region;
                }
                if (creature->entity_data.current_region != IntVector2(0, 0))
                {
                    return creature->entity_data.current_region;
                }
            }
        }
    }

    // Priority 3: Session initial spawn region in the Host's world
    const auto sessionSpawn = xp_progression::NetworkSync::GetSessionSpawnRegion();
    if (sessionSpawn.has_value())
    {
        return *sessionSpawn;
    }

    // Priority 4: Current player region in this session
    if (player->entity_data.current_region != IntVector2(0, 0))
    {
        return player->entity_data.current_region;
    }

    return IntVector2(0, 0);
}

int GetRegionDistance(IntVector2 region)
{
    const IntVector2 base_region = GetWorldBaseRegion();
    return xp_progression::CalculateChebyshevDistance(base_region, region);
}

int GetItemLevel(cube::Item* item)
{
    if (!item)
    {
        return 1;
    }

    cube::Game* game = cube::GetGame();
    if (!game)
    {
        return item->rarity;
    }

    return xp_progression::CalculateItemLevel(GetRegionDistance(item->region), item->modifier, item->rarity);
}

void SetEquipmentRegion(cube::Creature* creature, IntVector2 region)
{
    if (!creature)
    {
        return;
    }

    cube::Equipment* equipment = &creature->entity_data.equipment;
    equipment->chest.region = region;
    equipment->hands.region = region;
    equipment->feet.region = region;
    equipment->neck.region = region;
    equipment->pet.region = region;
    equipment->ring_left.region = region;
    equipment->ring_right.region = region;
    equipment->weapon_left.region = region;
    equipment->weapon_right.region = region;
    equipment->shoulder.region = region;
}

int GetCreatureLevel(cube::Creature* creature)
{
    if (!creature)
    {
        return 1;
    }

    cube::Game* game = cube::GetGame();
    if (!game)
    {
        return creature->entity_data.level;
    }

    IntVector2 current_region = creature->entity_data.current_region;

    // Spawned enemies without initialized region fallback to player region
    if (current_region == IntVector2(0, 0))
    {
        cube::Creature* player = game->GetPlayer();
        if (player)
        {
            current_region = player->entity_data.current_region;
        }
    }

    const int distance = GetRegionDistance(current_region);

    switch (creature->entity_data.hostility_type)
    {
    case cube::Creature::EntityBehaviour::Player:
    case cube::Creature::EntityBehaviour::Pet:
        return creature->entity_data.level;
    default:
        return xp_progression::CalculateCreatureLevel(distance, creature->id);
    }
}

int GetLevelVariation(long long modifier, int range)
{
    return xp_progression::GetLevelVariation(modifier, range);
}

unsigned long long ProgressionRand(unsigned long long seed)
{
    return xp_progression::ProgressionRand(seed);
}
