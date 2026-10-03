# RPCS3 libretro

[RPCS3](https://rpcs3.net/) (PlayStation 3 emulator) as a libretro core for RetroArch and other libretro frontends.

This repository builds the core and nothing else. The standalone Qt application, its packaging and the input and audio backends a frontend replaces are not part of it; the emulator itself tracks upstream RPCS3.

## Getting the core

Builds for Linux (x86_64 and aarch64), Windows x64 and Android arm64 are published as the [`libretro-latest`](https://github.com/WizzardSK/rpcs3-libretro/releases/tag/libretro-latest) release. Put the file for your platform in RetroArch's `cores` directory, together with `rpcs3_libretro.info` in its `info` directory.

The PS3 firmware (`PS3UPDAT.PUP`, from playstation.com) goes into RetroArch's system directory as `system/rpcs3/PS3UPDAT.PUP`; the core installs it on first start. Disc keys for redump ISOs go into `system/rpcs3/data/redump/`, named like the ISO with a `.key` or `.dkey` extension.

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
