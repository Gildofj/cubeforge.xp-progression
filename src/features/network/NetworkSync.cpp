#include "NetworkSync.h"
#include "../../core/Constants.h"
#include <vector>
#include <set>
#include <cstring>

namespace xp_progression {

    static std::optional<IntVector2> s_SyncedHostBaseRegion = std::nullopt;
    static std::optional<IntVector2> s_SessionSpawnRegion = std::nullopt;
    static uint64_t s_LastHostSteamID = 0;
    static std::set<uint64_t> s_KnownConnections;
    static uint32_t s_LastRequestTick = 0;

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

    std::vector<uint8_t> NetworkSync::SerializeHostBaseRegionPacket(IntVector2 baseRegion) {
        BytesIO writer;
        writer.Write<u32>(kPacketIdHostBaseRegionSync);
        writer.Write<i32>(baseRegion.x);
        writer.Write<i32>(baseRegion.y);
        return writer.Vector();
    }

    bool NetworkSync::DeserializeHostBaseRegionPacket(const uint8_t* buffer, size_t size, HostBaseRegionSyncPacket& outPacket) {
        if (!buffer || size < sizeof(uint32_t) + sizeof(int32_t) * 2) {
            return false;
        }

        BytesIO reader(const_cast<uint8_t*>(buffer), static_cast<int>(size));
        const uint32_t pkg_id = reader.Read<u32>();
        if (pkg_id != kPacketIdHostBaseRegionSync) {
            return false;
        }

        outPacket.packetId = pkg_id;
        outPacket.regionX = reader.Read<i32>();
        outPacket.regionY = reader.Read<i32>();
        return true;
    }

    std::vector<uint8_t> NetworkSync::SerializeRequestHostBaseRegionPacket() {
        BytesIO writer;
        writer.Write<u32>(kPacketIdRequestHostBaseRegion);
        return writer.Vector();
    }

    bool NetworkSync::DeserializeRequestHostBaseRegionPacket(const uint8_t* buffer, size_t size, RequestHostBaseRegionPacket& outPacket) {
        if (!buffer || size < sizeof(uint32_t)) {
            return false;
        }

        BytesIO reader(const_cast<uint8_t*>(buffer), static_cast<int>(size));
        const uint32_t pkg_id = reader.Read<u32>();
        if (pkg_id != kPacketIdRequestHostBaseRegion) {
            return false;
        }

        outPacket.packetId = pkg_id;
        return true;
    }

    bool NetworkSync::IsHost(cube::Game* game) {
        if (!game) return true;
        if (game->host.running) return true;
        if (!cube::SteamUser()) return true;

        const CSteamID mySteamID = cube::SteamUser()->GetSteamID();
        if (!game->client.host_steam_id.IsValid() || game->client.host_steam_id == mySteamID) {
            return true;
        }
        return false;
    }

    std::optional<IntVector2> NetworkSync::GetSyncedHostBaseRegion() {
        return s_SyncedHostBaseRegion;
    }

    void NetworkSync::SetSyncedHostBaseRegion(IntVector2 region) {
        s_SyncedHostBaseRegion = region;
    }

    void NetworkSync::ClearSyncedHostBaseRegion() {
        s_SyncedHostBaseRegion = std::nullopt;
    }

    std::optional<IntVector2> NetworkSync::GetSessionSpawnRegion() {
        return s_SessionSpawnRegion;
    }

    void NetworkSync::SetSessionSpawnRegion(IntVector2 region) {
        s_SessionSpawnRegion = region;
    }

    void NetworkSync::ClearSessionSpawnRegion() {
        s_SessionSpawnRegion = std::nullopt;
    }

    void NetworkSync::PollIncomingPackets(cube::Game* game) {
        if (!game || !cube::SteamNetworking() || !cube::SteamUser()) return;

        const CSteamID mySteamID = cube::SteamUser()->GetSteamID();
        uint32 packetSize = 0;
        while (cube::SteamNetworking()->IsP2PPacketAvailable(&packetSize, kP2PProgressionChannel)) {
            if (packetSize == 0) break;

            std::vector<uint8_t> buffer(packetSize);
            CSteamID senderSteamID;
            uint32 bytesRead = 0;

            if (cube::SteamNetworking()->ReadP2PPacket(buffer.data(), packetSize, &bytesRead, &senderSteamID, kP2PProgressionChannel)) {
                // Ignore packets sent by ourselves if any looped back
                if (senderSteamID == mySteamID) {
                    continue;
                }

                if (bytesRead < sizeof(uint32_t)) {
                    continue;
                }

                uint32_t packetId = 0;
                std::memcpy(&packetId, buffer.data(), sizeof(uint32_t));

                if (packetId == kPacketIdXPSync) {
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
                } else if (packetId == kPacketIdHostBaseRegionSync) {
                    HostBaseRegionSyncPacket packet;
                    if (DeserializeHostBaseRegionPacket(buffer.data(), bytesRead, packet)) {
                        SetSyncedHostBaseRegion(IntVector2(packet.regionX, packet.regionY));
                    }
                } else if (packetId == kPacketIdRequestHostBaseRegion) {
                    if (IsHost(game)) {
                        cube::Creature* hostPlayer = game->GetPlayer();
                        if (hostPlayer) {
                            IntVector2 hostBase = hostPlayer->entity_data.equipment.unk_item.region;
                            if (hostPlayer->entity_data.equipment.unk_item.modifier == 0 ||
                                (hostBase == IntVector2(0, 0) && hostPlayer->entity_data.current_region != IntVector2(0, 0))) {
                                hostBase = hostPlayer->entity_data.current_region;
                            }
                            SendHostBaseRegionTo(senderSteamID, hostBase);
                        }
                    }
                }
            }
        }
    }

    void NetworkSync::BroadcastXP(cube::Game* game, float totalXPGain) {
        if (!game || !cube::SteamUser() || !cube::SteamNetworking()) return;

        const CSteamID mySteamID = cube::SteamUser()->GetSteamID();

        // Count remote connections only
        size_t remoteCount = 0;
        for (const auto& conn : game->host.connections) {
            if (conn.first != mySteamID) {
                remoteCount++;
            }
        }

        if (remoteCount > 0) {
            const int xpPerPlayer = static_cast<int>(totalXPGain / static_cast<float>(remoteCount + 1));
            const std::vector<uint8_t> packetData = SerializeXPPacket(xpPerPlayer);

            for (const auto& conn : game->host.connections) {
                if (conn.first == mySteamID) {
                    continue; // Never send to ourselves
                }
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

    void NetworkSync::BroadcastHostBaseRegion(cube::Game* game, IntVector2 baseRegion) {
        if (!game || !cube::SteamUser() || !cube::SteamNetworking()) return;

        const CSteamID mySteamID = cube::SteamUser()->GetSteamID();
        const std::vector<uint8_t> packetData = SerializeHostBaseRegionPacket(baseRegion);

        for (const auto& conn : game->host.connections) {
            if (conn.first == mySteamID) {
                continue;
            }
            cube::SteamNetworking()->SendP2PPacket(
                conn.first,
                packetData.data(),
                static_cast<uint32>(packetData.size()),
                k_EP2PSendReliable,
                kP2PProgressionChannel
            );
        }
    }

    void NetworkSync::SendHostBaseRegionTo(CSteamID targetSteamID, IntVector2 baseRegion) {
        if (!cube::SteamNetworking()) return;

        const std::vector<uint8_t> packetData = SerializeHostBaseRegionPacket(baseRegion);
        cube::SteamNetworking()->SendP2PPacket(
            targetSteamID,
            packetData.data(),
            static_cast<uint32>(packetData.size()),
            k_EP2PSendReliable,
            kP2PProgressionChannel
        );
    }

    void NetworkSync::RequestHostBaseRegion(cube::Game* game) {
        if (!game || !cube::SteamNetworking() || IsHost(game)) return;

        if (game->client.host_steam_id.IsValid()) {
            const std::vector<uint8_t> packetData = SerializeRequestHostBaseRegionPacket();
            cube::SteamNetworking()->SendP2PPacket(
                game->client.host_steam_id,
                packetData.data(),
                static_cast<uint32>(packetData.size()),
                k_EP2PSendReliable,
                kP2PProgressionChannel
            );
        }
    }

    void NetworkSync::UpdateNetwork(cube::Game* game) {
        if (!game) return;

        if (IsHost(game)) {
            // Reset client tracking if we switched to host
            if (s_LastHostSteamID != 0) {
                s_LastHostSteamID = 0;
                ClearSyncedHostBaseRegion();
                ClearSessionSpawnRegion();
            }

            cube::Creature* player = game->GetPlayer();
            if (player && cube::SteamNetworking() && cube::SteamUser()) {
                const CSteamID mySteamID = cube::SteamUser()->GetSteamID();
                IntVector2 hostBase = player->entity_data.equipment.unk_item.region;
                if (player->entity_data.equipment.unk_item.modifier == 0 ||
                    (hostBase == IntVector2(0, 0) && player->entity_data.current_region != IntVector2(0, 0))) {
                    hostBase = player->entity_data.current_region;
                }

                // Check for newly joined clients and send them the base region
                std::set<uint64_t> currentConnections;
                for (const auto& conn : game->host.connections) {
                    if (conn.first == mySteamID) continue;

                    const uint64_t steamId64 = conn.first.ConvertToUint64();
                    currentConnections.insert(steamId64);

                    if (s_KnownConnections.find(steamId64) == s_KnownConnections.end()) {
                        // New client connected!
                        SendHostBaseRegionTo(conn.first, hostBase);
                    }
                }
                s_KnownConnections = std::move(currentConnections);
            }
        } else {
            // Client mode
            s_KnownConnections.clear();
            const uint64_t currentHostSteamID = game->client.host_steam_id.ConvertToUint64();

            // Track session spawn region
            cube::Creature* player = game->GetPlayer();
            if (player && player->entity_data.current_region != IntVector2(0, 0) && !s_SessionSpawnRegion.has_value()) {
                SetSessionSpawnRegion(player->entity_data.current_region);
            }

            // Host changed or joined a new world
            if (currentHostSteamID != s_LastHostSteamID) {
                s_LastHostSteamID = currentHostSteamID;
                ClearSyncedHostBaseRegion();
                ClearSessionSpawnRegion();
                if (player && player->entity_data.current_region != IntVector2(0, 0)) {
                    SetSessionSpawnRegion(player->entity_data.current_region);
                }
                RequestHostBaseRegion(game);
                s_LastRequestTick = GetTickCount();
            } else if (!s_SyncedHostBaseRegion.has_value()) {
                // Retry request every 2 seconds until received
                const uint32_t now = GetTickCount();
                if (now - s_LastRequestTick > 2000) {
                    s_LastRequestTick = now;
                    RequestHostBaseRegion(game);
                }
            }
        }
    }

} // namespace xp_progression
