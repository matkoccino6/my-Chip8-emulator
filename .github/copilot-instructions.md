# Copilot instructions for my-Chip8-emulator

This repo currently contains only a README ([README.md](README.md)). Use these repository-specific guidelines to be productive when adding, modifying, or reviewing emulator code.

**Purpose:**
- **Goal:** Implement or maintain a Chip-8 emulator (CPU, memory, display, input, timers, ROM loader).

**Where to start (discovery):**
- **Look for:** `src/`, `lib/`, `cmd/`, `roms/`, `tests/`, `Makefile`, `pyproject.toml`, `Cargo.toml`, `package.json`, `CMakeLists.txt`.
- **Entry points:** Search for files named `main.*`, `emulator.*`, `chip8.*`, or test suites under `tests/`.

**Architecture & patterns to expect and maintain:**
- **CPU loop:** A main fetch–decode–execute loop that reads 2-byte opcodes from memory and updates timers.
- **Opcode dispatch:** A single dispatcher using a `switch`/`match` or table of handler functions keyed by the top nibble (e.g., opcode >> 12). Keep handlers small and isolated.
- **State container:** Emulator state (registers V0–VF, I, PC, stack, memory, delay/sound timers, display buffer, keypad state) should be a single struct/class passed to handlers.
- **ROM loading:** ROMs are loaded into memory starting at 0x200; do not modify ROM bytes in-place.
- **Display:** 64x32 (or 64x32/128x64 variants) bitmap buffer; `draw_sprite` should XOR pixels and set VF on collisions.

**Testing & debugging:**
- **Test patterns:** Prefer small, deterministic unit tests for opcode handlers. Place tests in `tests/` or alongside modules.
- **Debugging hooks:** Add a `--step` or `--log-opcodes` mode rather than changing handler logic. Keep verbose logging opt-in.

**Conventions for changes and PRs:**
- **Small focused commits:** One opcode or subsystem per commit. Name commits like: "impl: OP_6XNN (LD Vx, byte)".
- **Examples & fixtures:** Put example ROMs in `roms/` and small test fixtures in `tests/fixtures/`.

**When information is missing:**
- If build, run, or test commands are not present, ask the repository maintainer for the preferred language/toolchain and CI commands before implementing non-trivial changes.

**Search examples for agent actions:**
- To find opcode implementations: search for `opcode`, `fetch`, `decode`, `V[0-9]`, or hex constants like `0x200`.
- To find timers/IO: search for `delay`, `sound`, `keypad`, `draw`, `sprite`.

If you need additional repository details (preferred language, CI, or example ROMs), ask the maintainer and I will incorporate them into this guidance.
