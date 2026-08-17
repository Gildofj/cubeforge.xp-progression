#include "../test_framework.h"
#include "../../src/features/scaling/ScalingSystem.h"
#include "../../src/core/StatTypes.h"

TEST_FUNC(StatScaling, DefaultScalingTablesRegression) {
    pyro::ScalingSystem scaling;

    const auto& playerTable = scaling.GetPlayerScaling();
    const auto& creatureTable = scaling.GetCreatureScaling();

    // Player baseline
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::HEALTH)], 1.0f, 0.0001f);
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::ARMOR)], 0.2f, 0.0001f);
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::RESISTANCE)], 0.2f, 0.0001f);
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::ATK_POWER)], 0.2f, 0.0001f);
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::SPELL_POWER)], 0.2f, 0.0001f);
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::CRIT)], 0.0001f, 0.00001f);
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::HASTE)], 0.0001f, 0.00001f);

    // CRITICAL REGRESSION TEST: Ensure stamina and mana are 0.05f and NOT overwritten by creature table to 0
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::STAMINA)], 0.05f, 0.0001f);
    ASSERT_NEAR(playerTable[pyro::ToIndex(pyro::StatType::MANA)], 0.05f, 0.0001f);

    // Creature baseline
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::HEALTH)], 2.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::ARMOR)], 0.01f, 0.0001f);
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::RESISTANCE)], 0.01f, 0.0001f);
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::ATK_POWER)], 3.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::SPELL_POWER)], 3.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::CRIT)], 0.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::HASTE)], 0.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::STAMINA)], 0.0f, 0.0001f);
    ASSERT_NEAR(creatureTable[pyro::ToIndex(pyro::StatType::MANA)], 0.0f, 0.0001f);
}

TEST_FUNC(StatScaling, CustomScalingOverrides) {
    pyro::ScalingSystem scaling;

    scaling.SetPlayerScaling(pyro::StatType::HEALTH, 5.0f);
    ASSERT_NEAR(scaling.GetPlayerScaling()[pyro::ToIndex(pyro::StatType::HEALTH)], 5.0f, 0.0001f);

    scaling.SetCreatureScaling(pyro::StatType::ATK_POWER, 10.0f);
    ASSERT_NEAR(scaling.GetCreatureScaling()[pyro::ToIndex(pyro::StatType::ATK_POWER)], 10.0f, 0.0001f);
}

void RegisterStatScalingTests() {
    REGISTER_TEST(StatScaling, DefaultScalingTablesRegression);
    REGISTER_TEST(StatScaling, CustomScalingOverrides);
}
