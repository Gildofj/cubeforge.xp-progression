#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include "cwsdk.h"
#include "../../core/Constants.h"

namespace xp_progression {

    struct XPSyncPacket {
        uint32_t packetId = kPacketIdXPSync;
        int32_t xpAmount = 0;
    };

    struct HostBaseRegionSyncPacket {
        uint32_t packetId = kPacketIdHostBaseRegionSync;
        int32_t regionX = 0;
        int32_t regionY = 0;
    };

    struct RequestHostBaseRegionPacket {
        uint32_t packetId = kPacketIdRequestHostBaseRegion;
    };

    class NetworkSync {
    public:
        /**
         * @brief Serializes an XP sync packet into a raw byte vector.
         */
        [[nodiscard]] static std::vector<uint8_t> SerializeXPPacket(int xpAmount);

        /**
         * @brief Parses an incoming P2P packet buffer into an XPSyncPacket.
         */
        [[nodiscard]] static bool DeserializeXPPacket(const uint8_t* buffer, size_t size, XPSyncPacket& outPacket);

        /**
         * @brief Serializes a host base region packet into a raw byte vector.
         */
        [[nodiscard]] static std::vector<uint8_t> SerializeHostBaseRegionPacket(IntVector2 baseRegion);

        /**
         * @brief Parses an incoming P2P packet buffer into a HostBaseRegionSyncPacket.
         */
        [[nodiscard]] static bool DeserializeHostBaseRegionPacket(const uint8_t* buffer, size_t size, HostBaseRegionSyncPacket& outPacket);

        /**
         * @brief Serializes a request host base region packet into a raw byte vector.
         */
        [[nodiscard]] static std::vector<uint8_t> SerializeRequestHostBaseRegionPacket();

        /**
         * @brief Parses an incoming P2P packet buffer into a RequestHostBaseRegionPacket.
         */
        [[nodiscard]] static bool DeserializeRequestHostBaseRegionPacket(const uint8_t* buffer, size_t size, RequestHostBaseRegionPacket& outPacket);

        /**
         * @brief Checks if the local player is currently hosting (or in singleplayer).
         */
        [[nodiscard]] static bool IsHost(cube::Game* game);

        /**
         * @brief Returns the synced host base region if received from the host.
         */
        [[nodiscard]] static std::optional<IntVector2> GetSyncedHostBaseRegion();
        static void SetSyncedHostBaseRegion(IntVector2 region);
        static void ClearSyncedHostBaseRegion();

        /**
         * @brief Returns the initial spawn region of this multiplayer session.
         */
        [[nodiscard]] static std::optional<IntVector2> GetSessionSpawnRegion();
        static void SetSessionSpawnRegion(IntVector2 region);
        static void ClearSessionSpawnRegion();

        /**
         * @brief Polls Steam P2P packets on channel 2 and applies incoming packets.
         */
        static void PollIncomingPackets(cube::Game* game);

        /**
         * @brief Broadcasts earned XP divided evenly across all connected peers.
         */
        static void BroadcastXP(cube::Game* game, float totalXPGain);

        /**
         * @brief Broadcasts host base region to all connected peers.
         */
        static void BroadcastHostBaseRegion(cube::Game* game, IntVector2 baseRegion);

        /**
         * @brief Sends host base region to a specific connected peer.
         */
        static void SendHostBaseRegionTo(CSteamID targetSteamID, IntVector2 baseRegion);

        /**
         * @brief Requests host base region from the remote host.
         */
        static void RequestHostBaseRegion(cube::Game* game);

        /**
         * @brief Updates network state, connection tracking, and periodic requests.
         */
        static void UpdateNetwork(cube::Game* game);
    };

} // namespace xp_progression
