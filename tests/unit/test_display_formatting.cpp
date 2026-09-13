#include "../test_framework.h"
#include "cwsdk.h"
#include "../../src/features/hud/HUDFormatter.h"
#include <string>
#include <cwchar>

TEST_FUNC(DisplayFormatting, ItemLevelTextUnits) {
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatLevelPrefix(1.0) == L"LV 1 ");
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatLevelPrefix(25.0) == L"LV 25 ");
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatLevelPrefix(999.0) == L"LV 999 ");

    ASSERT_TRUE(xp_progression::HUDFormatter::FormatLevelPrefix(1001.0) == L"LV 1.00K ");
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatLevelPrefix(50000.0) == L"LV 50.00K ");
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatLevelPrefix(999999.0) == L"LV 1000.00K ");

    ASSERT_TRUE(xp_progression::HUDFormatter::FormatLevelPrefix(1000001.0) == L"LV 1.00M ");
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatLevelPrefix(12345678.0) == L"LV 12.35M ");
}

TEST_FUNC(DisplayFormatting, ItemNamePrefixRules) {
    auto ApplyPrefix = [](int category, double level, std::wstring& itemName) -> bool {
        if (category < 3 || category > 9) return false;
        itemName = xp_progression::HUDFormatter::FormatLevelPrefix(level) + itemName;
        return true;
    };

    std::wstring weapon = L"Ruby Greatsword";
    bool renamed = ApplyPrefix(3, 15.0, weapon);
    ASSERT_TRUE(renamed);
    ASSERT_TRUE(weapon == L"LV 15 Ruby Greatsword");

    std::wstring chest = L"Obsidian Plate";
    renamed = ApplyPrefix(6, 2500.0, chest);
    ASSERT_TRUE(renamed);
    ASSERT_TRUE(chest == L"LV 2.50K Obsidian Plate");

    // Non-gear items (materials, coins, food)
    std::wstring potion = L"Life Potion";
    renamed = ApplyPrefix(1, 10.0, potion);
    ASSERT_FALSE(renamed);
    ASSERT_TRUE(potion == L"Life Potion");
}

TEST_FUNC(DisplayFormatting, RegionTierTextRules) {
    // Starting region
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatRegionBracket(0) == L"LV.1-5 ");

    // Distance 1
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatRegionBracket(1) == L"LV.6-10 ");

    // Distance 9 -> upper = 50 -> LV.46-50
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatRegionBracket(9) == L"LV.46-50 ");

    // Thousands tier (distance 200 -> upper 1005 -> 1K)
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatRegionBracket(200) == L"LV.1K ");

    // Millions tier (distance 200000 -> upper 1000005 -> 1M)
    ASSERT_TRUE(xp_progression::HUDFormatter::FormatRegionBracket(200000) == L"LV.1M ");
}

void RegisterDisplayFormattingTests() {
    REGISTER_TEST(DisplayFormatting, ItemLevelTextUnits);
    REGISTER_TEST(DisplayFormatting, ItemNamePrefixRules);
    REGISTER_TEST(DisplayFormatting, RegionTierTextRules);
}
