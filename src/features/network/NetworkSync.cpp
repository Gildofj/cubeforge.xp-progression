#include "NetworkSync.h"
#include "../../core/Constants.h"
#include <vector>

namespace pyro {

    std::vector<uint8_t> NetworkSync::SerializeXPPacket(int xpAmount) {
        BytesIO writer;
        writer.Write<u32>(kPacketIdXPSync);
        writer.Write<i32>(xpAmount);
        return writer.Vector();
    }

    bool NetworkSync::DeserializeXPPacket(const uint8_t* buffer, size_t size, XPSyncPacket& outPacket) {
        if (!buffer || size < sizeof(uint32_t) + sizeof(int32_t)) {
            return false;
        }

        BytesIO reader(const_cast<uint8_t*>(buffer), static_cast<int>(size));
        const uint32_t pkg_id = reader.Read<u32>();
        if (pkg_id != kPacketIdXPSync) {
            return false;
        }

        outPacket.packetId = pkg_id;
        outPacket.xpAmount = reader.Read<i32>();
        return true;
    }

    void NetworkSync::PollIncomingPackets(cube::Game* game) {
        if (!game || !cube::SteamNetworking()) return;

        uint32 packetSize = 0;
        while (cube::SteamNetworking()->IsP2PPacketAvailable(&packetSize, kP2PProgressionChannel)) {
            if (packetSize == 0) break;

            std::vector<uint8_t> buffer(packetSize);
            CSteamID senderSteamID;
            uint32 bytesRead = 0;

            if (cube::SteamNetworking()->ReadP2PPacket(buffer.data(), packetSize, &bytesRead, &senderSteamID, kP2PProgressionChannel)) {
                XPSyncPacket packet;
                if (DeserializeXPPacket(buffer.data(), bytesRead, packet)) {
                    cube::Creature* player = game->GetPlayer();
                    if (player && packet.xpAmount > 0) {
                        FloatRGBA purple(kColorPurpleR, kColorPurpleG, kColorPurpleB, kColorPurpleA);
                        wchar_t msgBuffer[64];
                        swprintf_s(msgBuffer, sizeof(msgBuffer)/sizeof(wchar_t), L"You gain %d xp.\n", packet.xpAmount);
                        game->PrintMessage(msgBuffer, &purple);

                        player->entity_data.XP += packet.xpAmount;
                    }
                }
            }
        }
    }

    void NetworkSync::BroadcastXP(cube::Game* game, float totalXPGain) {
        if (!game || !cube::SteamUser() || !cube::SteamNetworking()) return;

        const size_t connectionCount = game->host.connections.size();
        const bool isHostSession = (cube::SteamUser()->GetSteamID() == game->client.host_steam_id || connectionCount >= 1);

        if (isHostSession && connectionCount > 0) {
            const int xpPerPlayer = static_cast<int>(totalXPGain / static_cast<float>(connectionCount));
            const std::vector<uint8_t> packetData = SerializeXPPacket(xpPerPlayer);

            for (const auto& conn : game->host.connections) {
                cube::SteamNetworking()->SendP2PPacket(
                    conn.first,
                    packetData.data(),
                    static_cast<uint32>(packetData.size()),
                    k_EP2PSendReliable,
                    kP2PProgressionChannel
                );
            }
        }
    }

} // namespace pyro
