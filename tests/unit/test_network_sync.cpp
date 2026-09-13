#include "../test_framework.h"
#include "../../src/features/network/NetworkSync.h"
#include "../../src/core/Constants.h"

TEST_FUNC(NetworkSync, SerializeAndDeserializeXPPacket) {
    const int testXP = 2500;
    std::vector<uint8_t> buffer = xp_progression::NetworkSync::SerializeXPPacket(testXP);

    ASSERT_EQ(buffer.size(), (size_t)8); // 4 bytes packet ID + 4 bytes XP

    xp_progression::XPSyncPacket parsed;
    bool success = xp_progression::NetworkSync::DeserializeXPPacket(buffer.data(), buffer.size(), parsed);

    ASSERT_TRUE(success);
    ASSERT_EQ(parsed.packetId, xp_progression::kPacketIdXPSync);
    ASSERT_EQ(parsed.xpAmount, testXP);
}

TEST_FUNC(NetworkSync, DeserializeCorruptedOrIncompleteBuffer) {
    xp_progression::XPSyncPacket parsed;

    // Null buffer
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeXPPacket(nullptr, 8, parsed));

    // Too small buffer
    uint8_t smallBuf[4] = { 0x01, 0x00, 0x00, 0x00 };
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeXPPacket(smallBuf, sizeof(smallBuf), parsed));

    // Wrong packet ID
    uint8_t wrongIdBuf[8] = { 0x99, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00 };
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeXPPacket(wrongIdBuf, sizeof(wrongIdBuf), parsed));
}

void RegisterNetworkSyncTests() {
    REGISTER_TEST(NetworkSync, SerializeAndDeserializeXPPacket);
    REGISTER_TEST(NetworkSync, DeserializeCorruptedOrIncompleteBuffer);
}
