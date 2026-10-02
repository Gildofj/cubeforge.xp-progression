#pragma once

#include <cstdint>

namespace xp_progression {

    // Progression constants
    constexpr int kLevelsPerRegion = 5;
    constexpr int kLevelEquipmentCap = 0;

    // Steam P2P Network Channels & Packet IDs
    constexpr int kP2PProgressionChannel = 2;
    constexpr uint32_t kPacketIdXPSync = 0x01;
    constexpr uint32_t kPacketIdHostBaseRegionSync = 0x02;
    constexpr uint32_t kPacketIdRequestHostBaseRegion = 0x03;

    // Default combat text settings
    constexpr int32_t kTextFXLevelUpAnimLength = 3000;
    constexpr float kTextFXLevelUpDistance = -500.0f;
    constexpr float kTextFXLevelUpSize = 64.0f;

    constexpr int32_t kTextFXXPGainAnimLength = 3000;
    constexpr float kTextFXXPGainDistance = -100.0f;
    constexpr float kTextFXXPGainSize = 32.0f;

    // Color definitions (RGBA normalized floats)
    constexpr float kColorPurpleR = 0.65f;
    constexpr float kColorPurpleG = 0.40f;
    constexpr float kColorPurpleB = 1.00f;
    constexpr float kColorPurpleA = 1.00f;

} // namespace xp_progression
