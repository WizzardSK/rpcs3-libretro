# Changelog of the libretro core

What changed in the libretro core of this fork, newest first. Changes that come with a merge of upstream RPCS3 are in upstream's own history; a merge is noted here with the upstream build it brought in.

## Unreleased

- VBlank Frequency has a new choice, From List: a game named in `vblank_unlock.txt` runs with a 120 Hz VBlank - shown at 60 frames a second when it is a 30 fps game, at 120 when it is a 60 fps one - and any other game stays at 60 Hz. The list ships with the core and is written to `system/rpcs3/` when it is not there; the core never overwrites it, so titles can be added. It comes from the RPCS3 wiki's list of VBlank-compatible games and NNshi's testing.
- Frame Pacing is removed, option and code: the game's frames are timed by RPCS3's own clock, as in standalone. The other choice, one VBlank per frame RetroArch shows, added input lag and was better in no case tested.
- Save Data Slot is removed: the game's save list is always shown in the picture. The core picks the first entry itself only when the list cannot be shown.
- Default Resolution's tooltip warns against using it to upscale; Resolution Scale is for that.

## October 2026

### Upstream

- Merged up to upstream RPCS3 build 20243 (v0.0.43), after builds 20147 and 20149. The core reports upstream's version and build number, with this repository's commit after it, and logs both at start.

### Picture

- Vulkan through RetroArch's own hardware context: the renderer draws on RetroArch's device and hands each frame over as an image, with no copy through system memory. Finished frames are handed over in order, each once, so the picture no longer judders against RetroArch's refresh. The context is kept across RetroArch's video driver rebuilds, and replaced images stay alive until RetroArch has moved on.
- Before the first picture the Vulkan frame is 720p at the Resolution Scale, so the compilation progress is drawn sharp.
- A game no longer holds RetroArch through its PPU compilation at boot.
- Output Scaling and Vulkan Frame Readback are no longer options. Frame Pacing defaults to the emulator's clock.
- macOS: Vulkan through MoltenVK, no OpenGL in the core.

### Sound and pads

- Surround sound: the Audio Format option (Stereo by default, Surround 5.1, Surround 7.1) is what the PS3 reports to games, and with 5.1 or 7.1 the channels go to RetroArch's multi-channel output.
- Rumble through RetroArch's rumble interface.
- Pads report a PlayStation 3 controller, as standalone's handlers do, and the sticks reach full diagonals.

### Games and files

- PKG files are installed without being started, into the configured dev_hdd0, and the `.rap` licenses next to a PKG are installed with it.
- RPCS3's game patches (`patch.yml`) and its settings database ship with the core; Database Settings Override says whether the database's per-game settings apply. They, and the in-game overlays' icons, are rewritten when their contents differ.
- Save data is picked from the game's own list, shown in the picture.
- Emulate BD-ROM Read Speed as a core option.

### Core options

- The options carry RPCS3's own names and sections, in the layout NNshi proposed: CPU, GPU, Scheduling, Network, Debug, Core. Each option does what it says; several that were never read are now connected.
- Options for Disable Blit Engine Upscaling, RPCS3's performance overlay and the Resolution Scale Threshold.
- VBlank NTSC Fixup on by default, as in RPCS3.
- PSN Status says where an RPCN account comes from.
- On big.LITTLE ARM CPUs the compiler threads default to the big cores, and only the efficiency cores are left out.

### Builds

- macOS cores for Apple Silicon and Intel, and Android x86_64 next to arm64-v8a.
- The Windows core is `rpcs3_libretro.dll`; Linux links curl in instead of depending on Debian's libcurl-gnutls.
- Recipes for libretro's buildbot: Linux, Windows (cross-built with llvm-mingw), Android and macOS.

## Late September 2026

### Picture

- Vulkan is the default renderer, and the renderer option picks the renderer.
- Resolution Scale works: the scaled picture is carried through to RetroArch on both render paths and its size is reported.
- RPCS3's in-game dialogs are on and see the controller, including those that open before the game's first frame.
- Frame Limit Auto limits to the VBlank Rate, as in RPCS3; ZCULL Accuracy works; the resolution options are RPCS3's.
- A flip that waits forever for an image is reported and got out of, and the log says where the frames stopped when the picture does.

### Sound

- RetroArch gets one frame of audio per retro_run, at the rate the game played it, and is no longer starved of audio.

### Games and files

- Disc images boot: the ISO reader is ported from upstream, the game is found through its PARAM.SFO or PS3_DISC.SFB, and redump keys are found by the ISO's name.
- Games save and load without a save data dialog the core does not have; the save slot picked is the one chosen.
- Reset stops and boots the game the way unload and load do, and a game that restarts itself (exitspawn) keeps running.
- RPCS3's config and caches are under `system/rpcs3/`, and paths resolve the way standalone's do.
- Network options, with Winsock set up on Windows.

### Stability

- Closing content: the emulator finishes stopping before the disc goes away, the log writer thread is stopped in retro_deinit, and RPCS3's exception handlers are taken out on every unload on Windows.
- Windows fixes from NNshi's crash reports.

### The repository

- The Qt application, the standalone's input handlers and audio backends and the 7-Zip SDK are removed: the repository builds the core only.
- The core option texts are translated through libretro's Crowdin project.
- The core info file is published with the builds.

## Mid September 2026

- CI builds the core for Linux (x86_64, arm64), Windows and Android, with the libraries it needs bundled instead of taken from the host.
- A Vulkan renderer to choose besides OpenGL, and rendering without a hardware context through a headless swapchain.
- OpenGL: a context new enough for the extensions RPCS3 checks, GLEW linked in statically, and the RSX thread given a context of its own on X11.
- The core builds against both LLVM and both ffmpeg APIs.

## July 2026

- The first port: the RSX thread gets a real OpenGL context on Unix, core options are no longer discarded at boot, shared EGL contexts are not destroyed on the RSX thread, and the frontend is told when the guest exits on its own.
