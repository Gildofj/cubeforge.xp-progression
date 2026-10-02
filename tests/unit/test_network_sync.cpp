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

TEST_FUNC(NetworkSync, SerializeAndDeserializeHostBaseRegionPacket) {
    const IntVector2 testRegion(1234, -5678);
    std::vector<uint8_t> buffer = xp_progression::NetworkSync::SerializeHostBaseRegionPacket(testRegion);

    ASSERT_EQ(buffer.size(), (size_t)12); // 4 bytes packet ID + 4 bytes X + 4 bytes Y

    xp_progression::HostBaseRegionSyncPacket parsed;
    bool success = xp_progression::NetworkSync::DeserializeHostBaseRegionPacket(buffer.data(), buffer.size(), parsed);

    ASSERT_TRUE(success);
    ASSERT_EQ(parsed.packetId, xp_progression::kPacketIdHostBaseRegionSync);
    ASSERT_EQ(parsed.regionX, 1234);
    ASSERT_EQ(parsed.regionY, -5678);
}

TEST_FUNC(NetworkSync, DeserializeCorruptedHostBaseRegionPacket) {
    xp_progression::HostBaseRegionSyncPacket parsed;

    // Null buffer
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeHostBaseRegionPacket(nullptr, 12, parsed));

    // Too small buffer
    uint8_t smallBuf[8] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeHostBaseRegionPacket(smallBuf, sizeof(smallBuf), parsed));

    // Wrong packet ID
    uint8_t wrongIdBuf[12] = { 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeHostBaseRegionPacket(wrongIdBuf, sizeof(wrongIdBuf), parsed));
}

TEST_FUNC(NetworkSync, SerializeAndDeserializeRequestHostBaseRegionPacket) {
    std::vector<uint8_t> buffer = xp_progression::NetworkSync::SerializeRequestHostBaseRegionPacket();

    ASSERT_EQ(buffer.size(), (size_t)4); // 4 bytes packet ID

    xp_progression::RequestHostBaseRegionPacket parsed;
    bool success = xp_progression::NetworkSync::DeserializeRequestHostBaseRegionPacket(buffer.data(), buffer.size(), parsed);

    ASSERT_TRUE(success);
    ASSERT_EQ(parsed.packetId, xp_progression::kPacketIdRequestHostBaseRegion);
}

TEST_FUNC(NetworkSync, DeserializeCorruptedRequestHostBaseRegionPacket) {
    xp_progression::RequestHostBaseRegionPacket parsed;

    // Null buffer
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeRequestHostBaseRegionPacket(nullptr, 4, parsed));

    // Too small buffer
    uint8_t smallBuf[2] = { 0x03, 0x00 };
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeRequestHostBaseRegionPacket(smallBuf, sizeof(smallBuf), parsed));

    // Wrong packet ID
    uint8_t wrongIdBuf[4] = { 0x99, 0x00, 0x00, 0x00 };
    ASSERT_FALSE(xp_progression::NetworkSync::DeserializeRequestHostBaseRegionPacket(wrongIdBuf, sizeof(wrongIdBuf), parsed));
}

TEST_FUNC(NetworkSync, SyncedHostBaseRegionStateManagement) {
    xp_progression::NetworkSync::ClearSyncedHostBaseRegion();
    ASSERT_FALSE(xp_progression::NetworkSync::GetSyncedHostBaseRegion().has_value());

    xp_progression::NetworkSync::SetSyncedHostBaseRegion(IntVector2(42, -99));
    ASSERT_TRUE(xp_progression::NetworkSync::GetSyncedHostBaseRegion().has_value());
    ASSERT_EQ(xp_progression::NetworkSync::GetSyncedHostBaseRegion()->x, 42);
    ASSERT_EQ(xp_progression::NetworkSync::GetSyncedHostBaseRegion()->y, -99);

    xp_progression::NetworkSync::ClearSyncedHostBaseRegion();
    ASSERT_FALSE(xp_progression::NetworkSync::GetSyncedHostBaseRegion().has_value());
}

TEST_FUNC(NetworkSync, SessionSpawnRegionStateManagement) {
    xp_progression::NetworkSync::ClearSessionSpawnRegion();
    ASSERT_FALSE(xp_progression::NetworkSync::GetSessionSpawnRegion().has_value());

    xp_progression::NetworkSync::SetSessionSpawnRegion(IntVector2(100, 200));
    ASSERT_TRUE(xp_progression::NetworkSync::GetSessionSpawnRegion().has_value());
    ASSERT_EQ(xp_progression::NetworkSync::GetSessionSpawnRegion()->x, 100);
    ASSERT_EQ(xp_progression::NetworkSync::GetSessionSpawnRegion()->y, 200);

    xp_progression::NetworkSync::ClearSessionSpawnRegion();
    ASSERT_FALSE(xp_progression::NetworkSync::GetSessionSpawnRegion().has_value());
}

void RegisterNetworkSyncTests() {
    REGISTER_TEST(NetworkSync, SerializeAndDeserializeXPPacket);
    REGISTER_TEST(NetworkSync, DeserializeCorruptedOrIncompleteBuffer);
    REGISTER_TEST(NetworkSync, SerializeAndDeserializeHostBaseRegionPacket);
    REGISTER_TEST(NetworkSync, DeserializeCorruptedHostBaseRegionPacket);
    REGISTER_TEST(NetworkSync, SerializeAndDeserializeRequestHostBaseRegionPacket);
    REGISTER_TEST(NetworkSync, DeserializeCorruptedRequestHostBaseRegionPacket);
    REGISTER_TEST(NetworkSync, SyncedHostBaseRegionStateManagement);
    REGISTER_TEST(NetworkSync, SessionSpawnRegionStateManagement);
}
