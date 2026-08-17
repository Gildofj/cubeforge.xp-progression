# Memory Hooks & Reverse Engineering Reference

This document catalogs all memory offsets, detour hooks, assembly trampolines, and bytecode modifications performed by **PyroProgression** inside `cubeworld.exe`.

---

## 🧭 Table of Contents
1. [Memory Offset Map](#memory-offset-map)
2. [Bytecode Patches (NOP Overrides)](#bytecode-patches-nop-overrides)
3. [Assembly Trampolines & ABI Details](#assembly-trampolines--abi-details)
4. [Hook Subsystems](#hook-subsystems)
   - [XP Overwrite](#1-xp-overwrite)
   - [Level Display & Item Name Overwrites](#2-level-display--item-name-overwrites)
   - [Gear Scaling Overwrite](#3-gear-scaling-overwrite)
   - [Gold Drop Overwrite](#4-gold-drop-overwrite)
   - [Region Text Draw Overwrite](#5-region-text-draw-overwrite)
5. [Internal Game Functions Called](#internal-game-functions-called)

---

## Memory Offset Map

All offsets are relative to the base address of `cubeworld.exe` in the process address space (resolved via `CWOffset(address)`).

| Offset (RVA) | Target Function / Routine | Hook Type | Handler Symbol |
|---|---|---|---|
| `0x5FA80` | Native XP Required Calculation | Far JMP Detour | `ASM_XP_Overwrite` |
| `0x6688F` | Original Level Increment Routine | NOP Patch (10 bytes) | Inlined in `OnGameTick` |
| `0x668FA` | Original Level Stat Routine | NOP Patch (6 bytes) | Inlined in `OnGameTick` |
| `0xB1966` | Creature Nameplate Level Render | Far JMP Detour | `ASM_LevelDisplayOverwrite` |
| `0x16466C`| Item Tooltip Name String Builder | Far JMP Detour | `ASM_OverwriteItemName` |
| `0x109C50`| Base Gear Stat Scaling | Far JMP Detour | `GetGearScaling` |
| `0x10A490`| Equipment Haste Calculation | Far JMP Detour | `GetHasteRe` |
| `0x109F30`| Equipment HP Regen Calculation | Far JMP Detour | `GetRegenRe` |
| `0x1090F0`| Equipment Crit Calculation | Far JMP Detour | `GetCritRe` |
| `0x2A752C`| Creature Gold Drop Generation | Far JMP Detour | `ASM_OverwriteGoldDrops` |
| `0xABA58` | Top-Right Region Banner Render | Far JMP Detour | `ASM_RegionTextDrawOverwrite` |

---

## Bytecode Patches (NOP Overrides)

During the first execution of `OnGameTick`, PyroProgression disables the native artifact leveling system by overwriting opcodes with `0x90` (`NOP`):

```cpp
// Disable old leveling system
for (int i = 0; i < 10; i++)
{
    WriteByte(CWOffset(0x6688F + i), 0x90);
}
for (int i = 0; i < 6; i++)
{
    WriteByte(CWOffset(0x668FA + i), 0x90);
}
```

This prevents Cube World from resetting level progress or applying vanilla level calculations.

---

## Assembly Trampolines & ABI Details

Because Microsoft x64 ABI requires preserving non-volatile registers and maintaining 16-byte stack alignment prior to `CALL` instructions, PyroProgression uses standard macros:

### Context Macros
```nasm
#define PUSH_ALL "push rax\npush rbx\npush rcx\npush rdx\npush rsi\npush rdi\npush rbp\npush r8\npush r9\npush r10\npush r11\npush r12\npush r13\npush r14\npush r15\n"
#define POP_ALL  "pop r15\npop r14\npop r13\npop r12\npop r11\npop r10\npop r9\npop r8\npop rbp\npop rdi\npop rsi\npop rdx\npop rcx\npop rbx\npop rax\n"
```

### Stack Alignment Macros
```nasm
#define PREPARE_STACK "mov rax, rsp \n and rsp, 0xFFFFFFFFFFFFFFF0 \n push rax \n sub rsp, 0x28 \n"
#define RESTORE_STACK "add rsp, 0x28 \n pop rsp \n"
```

---

## Hook Subsystems

### 1. XP Overwrite
- **File**: [`src/XPOverwrite.h`](file:///d:/Projects/PyroProgression/src/XPOverwrite.h)
- **Target Offset**: `0x5FA80`
- **Behavior**: Intercepts the function computing required XP for a given level.
- **Trampoline**: `ASM_XP_Overwrite` allocates a 272-byte stack frame (`0x110`), saves all 16 SIMD registers (`xmm0`–`xmm15`), invokes `XP_Overwrite(int level)`, stores the result in `rax`, and restores SIMD state.

### 2. Level Display & Item Name Overwrites
- **File**: [`src/LevelDisplayOverwrite.h`](file:///d:/Projects/PyroProgression/src/LevelDisplayOverwrite.h)
- **Target Offsets**:
  - `0xB1966`: Hooks creature nametag rendering to display `LV <level>` over enemies.
  - `0x16466C`: Hooks tooltip text generation to prepend `LV <level>` to item names.
- **String Formatting**: Suffixes `K` (for levels $\ge 1,000$) and `M` (for levels $\ge 1,000,000$).

### 3. Gear Scaling Overwrite
- **File**: [`src/GearScalingOverWrite.h`](file:///d:/Projects/PyroProgression/src/GearScalingOverWrite.h)
- **Target Offsets**:
  - `0x109C50`: Main gear scaling routine.
  - `0x10A490`: Haste calculation.
  - `0x109F30`: Health regeneration calculation.
  - `0x1090F0`: Critical strike chance calculation.
- **Behavior**: Directly replaced by C calling convention exports (`GetGearScaling`, `GetHasteRe`, etc.).

### 4. Gold Drop Overwrite
- **File**: [`src/GoldDropOverWrite.h`](file:///d:/Projects/PyroProgression/src/GoldDropOverWrite.h)
- **Target Offset**: `0x2A752C`
- **Behavior**: Intercepts creature death drop routines, passes creature pointer and gold pointer to `GetGoldDrops`, and writes creature level to memory offset `0x2A7606`.

### 5. Region Text Draw Overwrite
- **File**: [`src/RegionTextDrawOverwrite.h`](file:///d:/Projects/PyroProgression/src/RegionTextDrawOverwrite.h)
- **Target Offset**: `0xABA58`
- **Behavior**: Injects regional level bracket (e.g. `LV.1-5 Ocean`, `LV.6-10 Hills`) into the `plasma::Node` rendering the top-right region title.

---

## Internal Game Functions Called

| Address / Offset | Signature / Prototype | Description |
|---|---|---|
| `0x486B0` | `void PutText(void* unk, wchar_t* buffer)` | Internal text renderer used for nametags |
| `0x336F0` | `void sub_336F0(void* a1, int a2, int a3)` | Internal node/text preparation routine |
