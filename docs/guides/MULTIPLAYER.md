# Multiplayer Progression & Networking Guide

This guide details how multiplayer progression, party XP sharing, and network synchronization operate in **CubeForge XP Progression** (`xp-progression`).

---

## 🧭 Table of Contents
1. [Overview](#overview)
2. [How XP Sharing Works](#how-xp-sharing-works)
3. [Network Protocol Details](#network-protocol-details)
4. [Host vs. Client Responsibilities](#host-vs-client-responsibilities)
5. [Multiplayer Best Practices](#multiplayer-best-practices)

---

## Overview

In vanilla Cube World, multiplayer progression relies solely on region-locked artifacts. `xp-progression` modifies this by introducing party-wide XP sharing across all connected players using Steam's peer-to-peer (P2P) networking layer.

---

## How XP Sharing Works

1. **Party Split**: When any enemy or boss is defeated by a player or pet, the host calculates total XP based on the creature's level, stars, and boss status.
2. **Fair Distribution**: The calculated XP is divided evenly across all active connections:
   $$\text{XP}_{\text{player}} = \left\lfloor \frac{\text{XP}_{\text{total}}}{N_{\text{connections}}} \right\rfloor$$
3. **Instant Synchronization**: Every client receives and adds the XP to their character progress immediately, triggering level ups and visual effects locally.

---

## Network Protocol Details

Communication occurs over **SteamNetworking Virtual Channel 2** using reliable transport (`k_EP2PSendReliable`).

### Packet Binary Structure

```text
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      Packet ID (u32: 0x01)                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      XP Amount (u32)                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

| Field | Type | Description |
|---|---|---|
| `Packet ID` | `uint32` | Identifier `0x01` denoting an XP award packet |
| `XP Amount` | `uint32` | Total XP to be awarded to the recipient |

---

## Host vs. Client Responsibilities

### Host Responsibilities
- Intercepts `OnCreatureDeath(cube::Game*, cube::Creature*, cube::Creature*)`.
- Verifies that the killer was a player or player pet.
- Computes XP gain and serializes the P2P packet.
- Broadcasts the packet across all connected peer Steam IDs.

### Client Responsibilities
- Polls `cube::SteamNetworking()->IsP2PPacketAvailable(&size, 2)` every tick in `OnGameTick`.
- Deserializes the packet using `BytesIO`.
- Invokes `GainXP(game, xp)` to update local entity data, create combat text FX, and handle any resulting level ups.

---

## Multiplayer Best Practices

- **Mod Compatibility**: All participating players should have the same version of `xp-progression` installed to ensure packet definitions and level formulas match.
- **Starting Out Together**: When starting a multiplayer world, explore together so your home regions and level curves stay synchronized.
- **Using `/recenter` in Parties**: If the party moves to a new distant region together, the host (and players) should run `/recenter` to adjust the world difficulty to their current location.
