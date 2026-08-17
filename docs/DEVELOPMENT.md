# Developer & Build Guide

This guide covers everything needed to set up a development environment, compile **PyroProgression** from source, and debug issues within Cube World.

---

## 🧭 Table of Contents
1. [Prerequisites](#prerequisites)
2. [Setting Up the Workspace](#setting-up-the-workspace)
3. [Building the Project](#building-the-project)
   - [Using Visual Studio (Recommended)](#using-visual-studio-recommended)
   - [Using CMake CLI](#using-cmake-cli)
4. [Deploying to Game](#deploying-to-game)
5. [Debugging & Troubleshooting](#debugging--troubleshooting)

---

## Prerequisites

- **Windows 10 / 11 (64-bit)**
- **CMake** (v3.8 or higher)
- **C++ Compiler Supporting x86_64 and Inline Assembly**:
  - **MSVC** (Visual Studio 2019 / 2022 with Desktop C++ workload) or **MinGW-w64 (GCC/Clang)**
  - Note: Code uses `asm(".intel_syntax ...")` and `__attribute__((naked))`, which are supported by GCC/Clang and modern Clang-cl / MSVC toolsets with GNU extensions.
- **Python 3.x** (optional, for running [`GenerateProjectCMake.py`](file:///d:/Projects/PyroProgression/GenerateProjectCMake.py))
- **Cube World (Steam Edition)** + [CubeModLoader](https://github.com/thetrueoneshots/Cube-World-Mod-Launcher/releases)
- **CWSDK (Cube World SDK)**: Submodule / repository providing `cwsdk.h` and shared definitions.

---

## Setting Up the Workspace

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/thetrueoneshots/PyroProgression.git
   cd PyroProgression
   ```

2. **Initialize / Populate CWSDK**:
   Ensure `CWSDK` directory contains the Cube World SDK files:
   ```bash
   git submodule update --init --recursive
   ```

3. **Verify CMakeLists.txt**:
   The root [`CMakeLists.txt`](file:///d:/Projects/PyroProgression/CMakeLists.txt) links `PyroProgression` against `CWSDK`.

---

## Building the Project

### Using Visual Studio (Recommended)

1. Open the project root folder in **Visual Studio** via `File -> Open -> Folder...`.
2. Visual Studio will automatically detect [`CMakeSettings.json`](file:///d:/Projects/PyroProgression/CMakeSettings.json) with `x64-Release` and `x64-Debug` configurations.
3. Select `x64-Release` from the target configuration dropdown.
4. Go to `Build -> Build All` (`Ctrl + Shift + B`).
5. Output binary will be generated at:
   ```text
   out/build/x64-Release/PyroProgression.dll
   ```

---

### Using CMake CLI

```bash
# Generate build files
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Compile DLL
cmake --build build --config Release
```

---

## Deploying to Game

1. Locate your Cube World installation directory (e.g., `C:\Program Files (x86)\Steam\steamapps\common\Cube World`).
2. Verify that `CubeModLoader.fip` is placed alongside `cubeworld.exe`.
3. Create a `Mods` directory if it does not already exist:
   ```text
   Cube World/
   ├── cubeworld.exe
   ├── CubeModLoader.fip
   └── Mods/
       └── PyroProgression.dll
   ```
4. Copy the compiled `PyroProgression.dll` into the `Mods/` folder.
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
> Do not set hardware/software breakpoints directly inside naked assembly stubs (`__attribute__((naked))`), as this can corrupt register states before preservation. Set breakpoints inside the invoked C/C++ target functions instead.
