#pragma once

#include <cstddef>

// bin/Icons/ui, compiled into the core by embed_ui_icons.cmake.
struct libretro_ui_icon
{
    const char* name;
    const unsigned char* data;
    std::size_t size;
};

extern const libretro_ui_icon g_libretro_ui_icons[];
extern const std::size_t g_libretro_ui_icon_count;
