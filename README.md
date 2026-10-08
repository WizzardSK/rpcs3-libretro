# RPCS3 libretro

[RPCS3](https://rpcs3.net/) (PlayStation 3 emulator) as a libretro core for RetroArch and other libretro frontends.

This repository builds the core and nothing else. The standalone Qt application, its packaging and the input and audio backends a frontend replaces are not part of it; the emulator itself tracks upstream RPCS3.

## Getting the core

Builds for Linux (x86_64 and aarch64), Windows x64, macOS (Apple Silicon and Intel) and Android (arm64 and x86_64) are published as the [`libretro-latest`](https://github.com/WizzardSK/rpcs3-libretro/releases/tag/libretro-latest) release. Put the file for your platform in RetroArch's `cores` directory, together with `rpcs3_libretro.info` in its `info` directory.

The PS3 firmware (`PS3UPDAT.PUP`, from playstation.com) goes into RetroArch's system directory as `system/rpcs3/PS3UPDAT.PUP`; the core installs it on first start. Disc keys for redump ISOs go into `system/rpcs3/data/redump/`, named like the ISO with a `.key` or `.dkey` extension.

## Loading games

- A game in folder form is loaded by its `PS3_DISC.SFB` (a dumped disc) or `PARAM.SFO` (an installed or PSN game). Loading its `EBOOT.BIN` works too, but every game's executable has that name, so RetroArch's per-game and per-folder options can't tell the games apart; per-game option overrides only work when the game is loaded by its `.SFB` or `.SFO`.
- Loading a `.pkg` installs it and boots the game. A PSN game's `.rap` license next to the `.pkg` is installed with it.
- RPCS3's log is `system/rpcs3/rpcs3_detailed.log`; a crash report goes to RetroArch's save directory as `rpcs3_libretro_crash.log`.

## Tips

- **Database Settings Override** (on by default) applies RPCS3's per-game settings database, which covers most games' known issues. For a game that still misbehaves, its page in RPCS3's [compatibility list](https://rpcs3.net/compatibility) says what else it needs; the [Vblank compatible games list](https://wiki.rpcs3.net/index.php?title=Vblank_compatible_games_list) shows which games can run above 60 FPS.
- The **Patch Manager** category lists the loaded game's patches and turns them on per game. It keeps its own settings: standalone RPCS3's `patch_config.yml` is not imported.
- Standalone RPCS3 scales its picture to the window with bilinear filtering; RetroArch scales with nearest neighbour unless told otherwise, which looks sharper and blockier. For the standalone look, turn on Settings > Video > Scaling > Bilinear Filtering (as a core override, so it applies to RPCS3 only), or prepend a `bilinear.slangp` shader.
- A handful of rarely needed options are not in the menu, as standalone RPCS3 keeps them out of its settings dialog, but can be set in the core's `.opt` file (`RPCS3.opt`).
- The PS3 user name is in `dev_hdd0/home/00000001/localusername`; edit it to change it.
- `dev_hdd0` and the other emulated drives can be moved outside RetroArch's folders by editing `vfs.yml` (in `system/rpcs3/`, or `system/rpcs3/config/` on Windows).
- `dev_hdd1` fills up with games' cache over time and RPCS3 never cleans it. Its contents can be deleted safely when it gets too big.
- A game that has built up a large SPU/shader cache can take a minute or more to close. Let it finish.
- PS3 peripherals beyond the DualShock 3 (guitars, PlayStation Move, cameras and the like) are not supported yet.

## Building

The Linux build runs in a container with the toolchain RPCS3's CI uses, through `.ci/build-libretro.sh`; see `.github/workflows/libretro.yml` for the exact invocation, and `.ci/build-windows-clang-libretro.sh` for Windows (MSYS2 clang64). A local build is a regular CMake build of the `rpcs3_libretro` target:

```bash
git clone --recursive https://github.com/WizzardSK/rpcs3-libretro.git
cd rpcs3-libretro
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DUSE_NATIVE_INSTRUCTIONS=OFF -DUSE_SYSTEM_CURL=ON -DUSE_LIBEVDEV=OFF
cmake --build build --target rpcs3_libretro
```

LLVM is needed for the PPU/SPU recompilers (`-DLLVM_DIR=...`, or `-DSTATIC_LINK_LLVM=OFF` with a shared `libLLVM` next to the core).

## License

Most files are licensed under the terms of the GNU GPL-2.0-only license; see the LICENSE file for details. Some files may be licensed differently; check the appropriate file headers.
