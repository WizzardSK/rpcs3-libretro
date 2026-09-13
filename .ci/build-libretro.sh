#!/bin/sh -ex

# Builds the libretro core rather than the Qt application. Everything here
# mirrors build-linux-aarch64.sh - same images, same toolchain, same LLVM at
# /opt/llvm - with the GUI switched off and the core copied out at the end.
# LLVM is linked statically, so the core is a single file with nothing to
# place beside it.
#
# Wayland is disabled deliberately. With it found, rpcs3/Emu links OpenGL::EGL,
# an imported target created by a find_package down in 3rdparty/ and therefore
# visible only in that directory - the generate step then fails with "target
# was not found". The core has no use for Wayland anyway: the frontend hands it
# the context.

cd rpcs3 || exit 1

git config --global --add safe.directory '*'

# Pull all the submodules except some. LLVM is deliberately left out: the image
# already carries a built one at /opt/llvm.
# shellcheck disable=SC2046
git submodule -q update --init $(awk '/path/ && !/llvm/ && !/opencv/ && !/libsdl-org/ && !/curl/ && !/zlib/ { print $3 }' .gitmodules)

mkdir -p build && cd build || exit 1

if [ "$COMPILER" = "gcc" ]; then
    # These are set in the dockerfile
    export CC="${GCC_BINARY}"
    export CXX="${GXX_BINARY}"
    export LINKER=gold
else
    export CC="${CLANG_BINARY}"
    export CXX="${CLANGXX_BINARY}"
    export LINKER="${LLD_BINARY}"
fi

export LINKER_FLAG="-fuse-ld=${LINKER}"

cmake ..                                               \
    -DCMAKE_BUILD_TYPE=Release                         \
    -DBUILD_LIBRETRO_CORE_ONLY=ON                      \
    -DUSE_NATIVE_INSTRUCTIONS=OFF                      \
    -DUSE_PRECOMPILED_HEADERS=OFF                      \
    -DCMAKE_EXE_LINKER_FLAGS="${LINKER_FLAG}"          \
    -DCMAKE_MODULE_LINKER_FLAGS="${LINKER_FLAG}"       \
    -DCMAKE_SHARED_LINKER_FLAGS="${LINKER_FLAG}"       \
    -DUSE_SYSTEM_CURL=ON                               \
    -DUSE_SDL=ON                                       \
    -DUSE_SYSTEM_SDL=ON                                \
    -DUSE_SYSTEM_FFMPEG=OFF                            \
    -DUSE_SYSTEM_OPENCV=ON                             \
    -DUSE_DISCORD_RPC=OFF                              \
    -DOpenGL_GL_PREFERENCE=LEGACY                      \
    -DCMAKE_DISABLE_FIND_PACKAGE_Wayland=ON            \
    -DLLVM_DIR=/opt/llvm/lib/cmake/llvm                \
    -DSTATIC_LINK_LLVM=ON                              \
    -G Ninja

ninja rpcs3_libretro

CORE=rpcs3/libretro/rpcs3_libretro.so
test -f "$CORE"

mkdir -p "$ARTDIR"
cp "$CORE" "$ARTDIR/"
# So the release says what it is without anyone having to open it.
"${STRIP_BINARY:-strip}" --strip-unneeded "$ARTDIR/rpcs3_libretro.so" || true
ls -la "$ARTDIR"
