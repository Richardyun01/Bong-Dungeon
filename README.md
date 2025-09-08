# Bong-Dungeon

Bong-Dungeon is a small C++ roguelike built on top of the [libtcod](https://libtcod.readthedocs.io/) toolkit and SDL2.  It features procedurally generated maps, turn‑based combat, items and inventory management, and persistence through save files.

## Features
- **Procedural dungeons** using BSP room generation and field-of-view computation.
- **Turn-based combat** against several enemy types.
- **Inventory & items** such as potions and ranged abilities.
- **Level progression** with stat boosts and a simple experience system.
- **Save/Load support** to continue runs later.

## Controls
- Arrow keys or numpad: movement
- `g`: pick up an item
- `d`: drop an item
- `i`: open inventory and use an item
- `/`: use stairs to descend to the next floor
- `Esc`: pause menu / exit

## Building
The project depends on `libtcod` and `SDL2`.

### Windows
A Code::Blocks project (`rogue.cbp`) and prebuilt libraries/DLLs are included. Open the project and build with the provided configuration.

### Other platforms
Install `libtcod` and `SDL2` from your system's package manager, then compile:

```bash
g++ src/*.cpp -Iinclude -I<path-to-libtcod> -I<path-to-sdl2> -ltcod -lSDL2 -o bong-dungeon
```

The static libraries shipped in `lib/` are built for Windows and cannot be linked on Linux.

## Running
A prebuilt `rogue.exe` binary is supplied for Windows users. Ensure `libtcod.dll` and `SDL2.dll` are present alongside the executable and run it from a terminal.

## Project structure
- `src/` – C++ source files
- `include/` – header files
- `lib/` – third‑party libraries
- `data/` – assets and save data
- `bin/` and `obj/` – build output directories

## License
Bong-Dungeon is distributed under the BSD 3‑Clause License; see [`LICENSE`](LICENSE) for details.

