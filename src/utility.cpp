#include "utility.h"

int GetRegionDistance(IntVector2 region)
{
    cube::Game* game = cube::GetGame();
    if (!game)
    {
        return 0;
    }

    cube::Creature* player = game->GetPlayer();
    if (!player)
    {
        return 0;
    }

    const IntVector2 base_region = player->entity_data.equipment.unk_item.region;
    return pyro::CalculateChebyshevDistance(base_region, region);
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

    return pyro::CalculateItemLevel(GetRegionDistance(item->region), item->modifier, item->rarity);
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
        return pyro::CalculateCreatureLevel(distance, creature->id);
    }
}

int GetLevelVariation(long long modifier, int range)
{
    return pyro::GetLevelVariation(modifier, range);
}

unsigned long long PyroRand(unsigned long long seed)
{
    return pyro::PyroRand(seed);
}
