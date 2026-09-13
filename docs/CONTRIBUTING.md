# Contributing Guidelines

Thank you for your interest in contributing to **xp-progression**! Open-source contributions from the community help keep Cube World modding alive and vibrant.

---

## 🧭 Table of Contents
1. [Code of Conduct](#code-of-conduct)
2. [How Can I Contribute?](#how-can-i-contribute)
3. [Development Workflow](#development-workflow)
4. [Coding Standards](#coding-standards)
5. [Pull Request Process](#pull-request-process)

---

## Code of Conduct

Please be respectful and constructive in all issues, pull requests, and discussions. We are all here to make Cube World more fun to play.

---

## How Can I Contribute?

- **Reporting Bugs**: Open an issue describing the bug, steps to reproduce, what you expected vs. what happened, and any logs or screenshots.
- **Balancing Adjustments**: Propose mathematical adjustments to XP rates, enemy stat formulas, or gear scaling in `src/GearScalingOverWrite.h` or `src/utility.cpp`.
- **Feature Enhancements**: Expand multiplayer synchronization, add new chat commands, or improve HUD readability.
- **Documentation**: Improve guides, fix typos, or clarify reverse-engineered memory documentation.

---

## Development Workflow

1. **Fork the Repository** on GitHub.
2. **Create a Feature Branch**:
   ```bash
   git checkout -b feature/my-new-feature
   ```
3. **Make Your Changes**:
   - Keep commits focused and atomic.
   - Follow the [Coding Standards](#coding-standards).
4. **Test In-Game**:
   - Verify that your build does not crash `cubeworld.exe` on startup, during combat, or when transitioning regions.
5. **Push and Open a Pull Request**:
   ```bash
   git push origin feature/my-new-feature
   ```

---

## Coding Standards

### C++ Conventions
- **Language Standard**: C++17 or C++20.
- **Naming Conventions**:
  - Class names: `PascalCase` (e.g. `Mod`, `MemoryHelper`)
  - Methods and Functions: `PascalCase` (e.g. `GetCreatureLevel`, `OnGameTick`)
  - Variables: `snake_case` or `camelCase` (e.g. `xp_gain`, `currentRegion`)
  - Member variables: `m_` prefix (e.g. `m_PlayerScaling`, `m_FXList`)
  - Constants and Macros: `ALL_CAPS` (e.g. `LEVELS_PER_REGION`, `LEVEL_EQUIPMENT_CAP`)

### Memory & Assembly Guidelines
- Always preserve all general purpose and XMM registers within naked assembly hooks using `PUSH_ALL` and `movaps` instructions.
- Align the stack to 16 bytes before calling C/C++ functions from assembly stubs using `PREPARE_STACK` and `RESTORE_STACK`.
- Never execute memory writes without verifying memory protection flags or using `VirtualProtect`.

---

## Pull Request Process

1. Provide a clear description of the problem solved or feature added.
2. If changing math formulas or memory offsets, explain the rationale and provide before/after values or game behavior.
3. Ensure documentation in `docs/` is updated in the same PR to reflect any formula or hook changes.
