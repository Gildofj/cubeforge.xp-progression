# Developer & Build Guide

This guide covers everything needed to set up a development environment, compile **xp-progression** from source, and debug issues within Cube World.

---

## 🧭 Table of Contents
1. [Prerequisites](#prerequisites)
2. [Setting Up the Workspace](#setting-up-the-workspace)
3. [Building the Project](#building-the-project)
   - [Using PowerShell Build Script (Recommended)](#using-powershell-build-script-recommended)
   - [Using CMake CLI with Presets](#using-cmake-cli-with-presets)
4. [Deploying to Game](#deploying-to-game)
5. [Debugging & Troubleshooting](#debugging--troubleshooting)

---

## Prerequisites

- **Windows 10 / 11 (64-bit)**
- **CMake** (v3.25 or higher)
- **C++ Compiler Supporting C++20 & x86_64 MASM**:
  - **MSVC** (Visual Studio 2022 with Desktop C++ workload) or **Clang**
- **Cube World (Steam Edition)** + [CubeForge Loader](https://github.com/Gildofj/cubeforge.loader/releases)
- **CubeForge SDK (CWSDK)**: Automatically resolved via `FetchContent` or local sibling directory.

---

## Setting Up the Workspace

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/Gildofj/cubeforge.xp-progression.git
   cd cubeforge.xp-progression
   ```

2. **Verify CMakeLists.txt**:
   The root [`CMakeLists.txt`](file:///d:/Projects/cubeforge.xp-progression/CMakeLists.txt) links `xp-progression` against `CWSDK`.

---

## Building the Project

### Using PowerShell Build Script (Recommended)

```powershell
# Build entire project (Release)
.\build.ps1 -Target all -BuildType Release

# Build and run automated test suite
.\build.ps1 -Target test

# Automatically build and install into Cube World Mods directory
.\build.ps1 -Target mod -InstallPath "D:\SteamLibrary\steamapps\common\Cube World"
```

The output DLL is generated in `dist/xp-progression.dll`.

---

### Using CMake CLI with Presets

```bash
# Configure debug / release presets
cmake --preset windows-release

# Compile DLL
cmake --build --preset windows-release --target xp-progression
```

---

## Deploying to Game

1. Locate your Cube World installation directory (e.g., `C:\Program Files (x86)\Steam\steamapps\common\Cube World`).
2. Verify that `CubeForgeLoader.fip` is placed alongside `cubeworld.exe`.
3. Create a `Mods` directory if it does not already exist:
   ```text
   Cube World/
   ├── cubeworld.exe
   ├── CubeForgeLoader.fip
   └── Mods/
       └── xp-progression.dll
   ```
4. Copy the compiled `xp-progression.dll` into the `Mods/` folder.
5. Launch `cubeworld.exe`.

---

## Debugging & Troubleshooting

### Enabling In-Game Logs / Messages
The mod utilizes `cube::Game::PrintMessage` for in-game feedback:
```cpp
cube::Game* game = cube::GetGame();
FloatRGBA color(1.0f, 0.0f, 0.0f, 1.0f);
game->PrintMessage(L"[Debug] Custom log message\n", &color);
```

### Visual Studio Live Debugging
1. Launch Cube World with the mod loaded.
2. In Visual Studio, go to `Debug -> Attach to Process...`.
3. Select `cubeworld.exe`.
4. Set breakpoints inside C++ hook handlers (e.g. `OnCreatureDeath`, `OnGameTick`, `GetGearScaling`).

> [!CAUTION]
> Do not set hardware/software breakpoints directly inside naked assembly stubs (`trampolines.asm`), as this can corrupt register states before preservation. Set breakpoints inside the invoked C/C++ target functions instead.
