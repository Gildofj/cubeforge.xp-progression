#include "../test_framework.h"
#include "../../src/features/scaling/ScalingSystem.h"
#include "../../src/core/StatTypes.h"

TEST_FUNC(StatScaling, DefaultScalingTablesRegression) {
    xp_progression::ScalingSystem scaling;

    const auto& playerTable = scaling.GetPlayerScaling();
    const auto& creatureTable = scaling.GetCreatureScaling();

    // Player baseline
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::HEALTH)], 1.0f, 0.0001f);
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::ARMOR)], 0.2f, 0.0001f);
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::RESISTANCE)], 0.2f, 0.0001f);
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::ATK_POWER)], 0.2f, 0.0001f);
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::SPELL_POWER)], 0.2f, 0.0001f);
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::CRIT)], 0.0001f, 0.00001f);
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::HASTE)], 0.0001f, 0.00001f);

    // CRITICAL REGRESSION TEST: Ensure stamina and mana are 0.05f and NOT overwritten by creature table to 0
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::STAMINA)], 0.05f, 0.0001f);
    ASSERT_NEAR(playerTable[xp_progression::ToIndex(xp_progression::StatType::MANA)], 0.05f, 0.0001f);

    // Creature baseline
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::HEALTH)], 2.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::ARMOR)], 0.01f, 0.0001f);
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::RESISTANCE)], 0.01f, 0.0001f);
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::ATK_POWER)], 3.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::SPELL_POWER)], 3.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::CRIT)], 0.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::HASTE)], 0.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::STAMINA)], 0.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[xp_progression::ToIndex(xp_progression::StatType::MANA)], 0.0f, 0.0001f);
}

TEST_FUNC(StatScaling, CustomScalingOverrides) {
    xp_progression::ScalingSystem scaling;

    scaling.SetPlayerScaling(xp_progression::StatType::HEALTH, 5.0f);
    ASSERT_NEAR(scaling.GetPlayerScaling()[xp_progression::ToIndex(xp_progression::StatType::HEALTH)], 5.0f, 0.0001f);

    scaling.SetCreatureScaling(xp_progression::StatType::ATK_POWER, 10.0f);
    ASSERT_NEAR(scaling.GetCreatureScaling()[xp_progression::ToIndex(xp_progression::StatType::ATK_POWER)], 10.0f, 0.0001f);
}

void RegisterStatScalingTests() {
    REGISTER_TEST(StatScaling, DefaultScalingTablesRegression);
    REGISTER_TEST(StatScaling, CustomScalingOverrides);
}
