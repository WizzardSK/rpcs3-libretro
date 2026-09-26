# Define GNU standard installation directories
include(GNUInstallDirs)

# Generate git-version.h at build time.
include(${CMAKE_CURRENT_SOURCE_DIR}/git-version.cmake)

# Check for a sufficient compiler and set build options
include(ConfigureCompiler)
include(CheckFunctionExists)

set(CMAKE_CXX_STANDARD 20)

if(UNIX AND NOT APPLE AND NOT ANDROID)
    add_compile_definitions(DATADIR="${CMAKE_INSTALL_FULL_DATADIR}/rpcs3")
    # Optionally enable X11 for window management
    find_package(X11)
    if(X11_FOUND)
        add_compile_definitions(HAVE_X11)
    endif()
    find_package(Wayland)
    if(WAYLAND_FOUND)
        add_compile_definitions(HAVE_WAYLAND)
    endif()
endif()

# This repository builds the libretro core and nothing else: the Qt
# application, its executable, packaging and unit tests are gone.
add_subdirectory(Emu)

gen_git_version(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(libretro)
