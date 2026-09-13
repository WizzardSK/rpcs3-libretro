#!/bin/sh -ex

# Builds the libretro core on Windows, under MSYS2's clang64 environment.
#
# Not MSVC: RPCS3 builds Windows two ways, msbuild over rpcs3.sln and this one,
# and the core exists only as a CMake target so the solution is no use. MSVC
# with CMake does not work either - the LLVM on the runner image is the client
# distribution, with no CMake package and no static libraries, so
# find_package(LLVM) finds nothing. clang64 has all of it as packages: LLVM
# with its CMake config, and ffmpeg, OpenCV, curl and GLEW besides, which is
# why this asks for the system copies of those.

git config --global --add safe.directory '*'

# The ones clang64 provides as packages are left out, as is LLVM.
# shellcheck disable=SC2046
git submodule -q update --init --depth 1 $(awk '/path/ && !/llvm/ && !/opencv/ && !/ffmpeg/ && !/curl/ && !/FAudio/ && !/zlib/ { print $3 }' .gitmodules)

mkdir -p build && cd build || exit 1

export CC="clang"
export CXX="clang++"
export LINKER=lld
export LINKER_FLAG="-fuse-ld=${LINKER}"
export AR="llvm-ar"
export RANLIB="llvm-ranlib"

cmake ..                                               \
    -DBUILD_LIBRETRO_CORE_ONLY=ON                      \
    -DCMAKE_BUILD_TYPE=Release                         \
    -DCMAKE_PREFIX_PATH=/clang64                       \
    -DUSE_NATIVE_INSTRUCTIONS=OFF                      \
    -DUSE_PRECOMPILED_HEADERS=OFF                      \
    -DCMAKE_EXE_LINKER_FLAGS="${LINKER_FLAG}"          \
    -DCMAKE_MODULE_LINKER_FLAGS="${LINKER_FLAG}"       \
    -DCMAKE_SHARED_LINKER_FLAGS="${LINKER_FLAG}"       \
    -DCMAKE_AR="$AR"                                   \
    -DCMAKE_RANLIB="$RANLIB"                           \
    -DUSE_SYSTEM_CURL=ON                               \
    -DUSE_FAUDIO=OFF                                   \
    -DUSE_SDL=OFF                                      \
    -DUSE_SYSTEM_FFMPEG=ON                             \
    -DUSE_SYSTEM_OPENCV=ON                             \
    -DUSE_SYSTEM_OPENAL=OFF                            \
    -DUSE_DISCORD_RPC=OFF                              \
    -DUSE_LTO=OFF                                      \
    -DOpenGL_GL_PREFERENCE=LEGACY                      \
    -DWITH_LLVM=ON                                     \
    -DLLVM_DIR=/clang64/lib/cmake/llvm                 \
    -DVulkan_LIBRARY=/clang64/lib/libvulkan-1.dll.a    \
    -DSTATIC_LINK_LLVM=ON                              \
    -G Ninja

ninja rpcs3_libretro

cd ..
mkdir -p artifacts
find build -name 'rpcs3_libretro.dll' -exec cp {} artifacts/rpcs3_libretro_windows_x86_64.dll \;
ls -la artifacts
