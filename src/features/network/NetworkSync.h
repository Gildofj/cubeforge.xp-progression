#pragma once

#include <cstdint>
#include <vector>
#include "cwsdk.h"
#include "../../core/Constants.h"

namespace pyro {

    struct XPSyncPacket {
        uint32_t packetId = kPacketIdXPSync;
        int32_t xpAmount = 0;
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
         * @brief Polls Steam P2P packets on channel 2 and applies incoming XP.
         */
        static void PollIncomingPackets(cube::Game* game);

        /**
         * @brief Broadcasts earned XP divided evenly across all connected peers.
         */
        static void BroadcastXP(cube::Game* game, float totalXPGain);
    };

} // namespace pyro
