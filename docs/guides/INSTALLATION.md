# Installation & Setup Guide

This guide walks you through installing and configuring **PyroProgression** for Cube World (Steam Edition).

---

## 🧭 Table of Contents
1. [System Requirements](#system-requirements)
2. [Prerequisites](#prerequisites)
3. [Step-by-Step Installation](#step-by-step-installation)
4. [Verifying Installation](#verifying-installation)
5. [Troubleshooting & Common Issues](#troubleshooting--common-issues)
6. [Uninstalling](#uninstalling)

---

## System Requirements

- **Game**: Cube World (Steam Release, 64-bit)
- **OS**: Windows 10 or Windows 11 (64-bit)
- **Mod Loader**: [CubeModLoader](https://github.com/thetrueoneshots/Cube-World-Mod-Launcher/releases) (`CubeModLoader.fip`)

---

## Prerequisites

Before installing PyroProgression, ensure you have CubeModLoader properly installed in your Cube World root directory.

---

## Step-by-Step Installation

### Step 1: Download the Release
1. Go to the [PyroProgression Releases Page](https://github.com/thetrueoneshots/PyroProgression/releases).
2. Download the latest `PyroProgression_v.x.x.zip`.

### Step 2: Locate Game Directory
1. Open **Steam**.
2. Right-click **Cube World** in your library.
3. Select `Manage -> Browse local files`.
4. This will open the directory containing `cubeworld.exe`.

### Step 3: Extract Files
1. If the `.zip` archive contains `CubeModLoader.fip`, place it directly in the root game directory (alongside `cubeworld.exe`).
2. Inside your game directory, create a folder named `Mods` (case-insensitive) if one does not already exist.
3. Place `PyroProgression.dll` inside the `Mods` folder.

```text
📁 Cube World/
├── 📄 cubeworld.exe
├── 📄 CubeModLoader.fip
└── 📁 Mods/
    └── 📄 PyroProgression.dll
```

---

## Verifying Installation

1. Launch Cube World via Steam or by running `cubeworld.exe`.
2. Start or load a world.
3. Check the bottom of the screen: the **XP bar** should now be visible in the center.
4. Kill an enemy: you should see a purple chat notification `You gain X xp.` and floating combat text `+X XP`.
5. Check the top-right corner: the region banner will display `LV.1-5 <Biome Name>`.

---

## Troubleshooting & Common Issues

| Issue | Probable Cause | Solution |
|---|---|---|
| **Game crashes on startup** | Outdated modloader or conflicting mod | Ensure `CubeModLoader.fip` is updated. Remove other DLLs from `Mods/` to isolate the conflict. |
| **No XP bar appears** | Mod not loaded by CubeModLoader | Verify `PyroProgression.dll` is inside the `Mods/` folder and `CubeModLoader.fip` is in the game root. |
| **Region level shows incorrect numbers** | Character has not initialized home region | Move between blocks or type `/recenter` in chat to initialize. |
| **Multiplayer XP not synced** | Clients or host missing the mod | Ensure all players in the session have PyroProgression installed. |

---

## Uninstalling

To remove PyroProgression:
1. Open your `Cube World/Mods/` folder.
2. Delete `PyroProgression.dll`.
3. The game will return to vanilla behavior upon the next launch.
