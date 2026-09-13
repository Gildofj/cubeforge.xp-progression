#include "../test_framework.h"
#include "cwsdk.h"
#include "../../src/utility.h"
#include <cmath>

#define TEST_LEVELS_PER_REGION 5

// Mirroring the core progression math formulas for isolated unit testing
inline int CalculateXPOverwrite(int level) {
    return (int)(50 * (1 + std::powf((float)level, 1.3f)));
}

inline int XorShift32Bits(int mod) {
    int mod1 = mod ^ (mod << 0x0D);
    int mod2 = mod1 ^ (mod1 >> 0x11);
    int mod3 = mod2 ^ (mod2 << 0x05);
    return mod3;
}

TEST_FUNC(ProgressionFormulas, PyroRandDeterminism) {
    // Deterministic random numbers
    ASSERT_EQ(PyroRand(0), (unsigned long long)0);
    ASSERT_EQ(PyroRand(123456), PyroRand(123456));
    ASSERT_EQ(PyroRand(0xABCDEF), PyroRand(0xABCDEF));

    // Must be bounded within 15-bit integer space (0 to 32767)
    for (unsigned long long s = 0; s < 1000; ++s) {
        unsigned long long val = PyroRand(s);
        ASSERT_LT(val, (unsigned long long)32768);
    }
}

TEST_FUNC(ProgressionFormulas, LevelVariationBounds) {
    int range = TEST_LEVELS_PER_REGION;
    for (long long mod = 0; mod < 500; ++mod) {
        int var = GetLevelVariation(mod, range);
        ASSERT_GE(var, 0);
        ASSERT_LT(var, range);
    }
}

TEST_FUNC(ProgressionFormulas, RegionDistanceCalculations) {
    // Chebyshev distance metric: max(|dx|, |dy|)
    auto CalcDist = [](IntVector2 r1, IntVector2 r2) -> int {
        return std::max<int>(std::abs(r1.x - r2.x), std::abs(r1.y - r2.y));
    };

    ASSERT_EQ(CalcDist(IntVector2(0, 0), IntVector2(0, 0)), 0);
    ASSERT_EQ(CalcDist(IntVector2(0, 0), IntVector2(5, 0)), 5);
    ASSERT_EQ(CalcDist(IntVector2(0, 0), IntVector2(-10, 0)), 10);
    ASSERT_EQ(CalcDist(IntVector2(0, 0), IntVector2(3, 8)), 8);
    ASSERT_EQ(CalcDist(IntVector2(-5, -5), IntVector2(5, 5)), 10);
    ASSERT_EQ(CalcDist(IntVector2(100, -200), IntVector2(150, -180)), 50);
}

TEST_FUNC(ProgressionFormulas, XPOverwriteFormulaAndGrowth) {
    // Level 1 baseline
    ASSERT_EQ(CalculateXPOverwrite(1), 100);

    // Monotonicity check across levels 1 to 200
    int prevXp = 0;
    for (int lvl = 1; lvl <= 200; ++lvl) {
        int currentXp = CalculateXPOverwrite(lvl);
        ASSERT_GT(currentXp, prevXp);
        prevXp = currentXp;
    }

    // Key milestone calculations
    ASSERT_EQ(CalculateXPOverwrite(10), 1047);
    ASSERT_EQ(CalculateXPOverwrite(50), 8134);
    ASSERT_EQ(CalculateXPOverwrite(100), 19955);
}

TEST_FUNC(ProgressionFormulas, EquipmentRegionSetter) {
    alignas(cube::Creature) uint8_t creatureBuffer[sizeof(cube::Creature)]{};
    auto* dummyCreature = reinterpret_cast<cube::Creature*>(creatureBuffer);
    IntVector2 targetRegion(42, -99);

    SetEquipmentRegion(dummyCreature, targetRegion);

    ASSERT_TRUE(dummyCreature->entity_data.equipment.chest.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.hands.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.feet.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.neck.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.pet.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.ring_left.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.ring_right.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.weapon_left.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.weapon_right.region == targetRegion);
    ASSERT_TRUE(dummyCreature->entity_data.equipment.shoulder.region == targetRegion);
}

TEST_FUNC(ProgressionFormulas, ItemCategoryClassification) {
    // Item category filters for progression stat calculations
    auto IsWeaponOrArmor = [](int cat) -> bool { return cat >= 3 && cat <= 9; };
    auto IsHasteEligible = [](int cat) -> bool { return cat >= 3 && cat <= 9; };
    auto IsRegenEligible = [](int cat) -> bool { return (cat >= 4 && cat <= 9) || cat == 26; };
    auto IsCritEligible = [](int cat) -> bool { return cat >= 3 && cat <= 10; };

    // Weapon/Armor range
    ASSERT_FALSE(IsWeaponOrArmor(2));
    ASSERT_TRUE(IsWeaponOrArmor(3));
    ASSERT_TRUE(IsWeaponOrArmor(5));
    ASSERT_TRUE(IsWeaponOrArmor(9));
    ASSERT_FALSE(IsWeaponOrArmor(10));

    // Haste
    ASSERT_TRUE(IsHasteEligible(3));
    ASSERT_TRUE(IsHasteEligible(9));
    ASSERT_FALSE(IsHasteEligible(1));
    ASSERT_FALSE(IsHasteEligible(10));

    // Regen
    ASSERT_FALSE(IsRegenEligible(3));
    ASSERT_TRUE(IsRegenEligible(4));
    ASSERT_TRUE(IsRegenEligible(9));
    ASSERT_FALSE(IsRegenEligible(10));
    ASSERT_TRUE(IsRegenEligible(26));

    // Crit
    ASSERT_TRUE(IsCritEligible(3));
    ASSERT_TRUE(IsCritEligible(10));
    ASSERT_FALSE(IsCritEligible(2));
    ASSERT_FALSE(IsCritEligible(11));
}

TEST_FUNC(ProgressionFormulas, XorShiftScrambling) {
    int testSeed = 0x55AA33CC;
    int res = XorShift32Bits(testSeed);
    ASSERT_NE(res, testSeed);
    ASSERT_EQ(XorShift32Bits(testSeed), res); // Pure function determinism
}

TEST_FUNC(ProgressionFormulas, GoldDropByteSplitting) {
    int goldValue = 0x12345678;
    unsigned char b0 = goldValue & 0xFF;
    unsigned char b1 = (goldValue >> 8) & 0xFF;
    unsigned char b2 = (goldValue >> 16) & 0xFF;
    unsigned char b3 = (goldValue >> 24) & 0xFF;

    ASSERT_EQ(b0, (unsigned char)0x78);
    ASSERT_EQ(b1, (unsigned char)0x56);
    ASSERT_EQ(b2, (unsigned char)0x34);
    ASSERT_EQ(b3, (unsigned char)0x12);
}

void RegisterProgressionFormulasTests() {
    REGISTER_TEST(ProgressionFormulas, PyroRandDeterminism);
    REGISTER_TEST(ProgressionFormulas, LevelVariationBounds);
    REGISTER_TEST(ProgressionFormulas, RegionDistanceCalculations);
    REGISTER_TEST(ProgressionFormulas, XPOverwriteFormulaAndGrowth);
    REGISTER_TEST(ProgressionFormulas, EquipmentRegionSetter);
    REGISTER_TEST(ProgressionFormulas, ItemCategoryClassification);
    REGISTER_TEST(ProgressionFormulas, XorShiftScrambling);
    REGISTER_TEST(ProgressionFormulas, GoldDropByteSplitting);
}
