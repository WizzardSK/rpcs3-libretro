#pragma once

#include "Emu/Io/MouseHandler.h"

#include <array>

// The PS3 mice, driven by RetroArch's light guns: one mouse per port, at the
// point the gun aims at, its trigger, second trigger and third button as the
// mouse's buttons 1 to 3 and the port's mouse wheel as its wheel. A gun is
// what the System 357 arcade shooters read through the USIO board, and
// RetroArch also makes one of the mouse, so a mouse works the same way.
//
// Standalone RPCS3 has one mouse connected by default (its basic handler);
// this is its raw handler's layout, a mouse per player, which the USIO board
// needs for two guns.
class LibretroMouseHandler final : public MouseHandlerBase
{
    using MouseHandlerBase::MouseHandlerBase;

public:
    void Init(const u32 max_connect) override;

    // Hand the guns polled for this frame to the mice; retro_run, after polling
    void update();

private:
    struct last_state
    {
        s32 x = -1;
        s32 y = -1;
        u8 buttons = 0;
    };
    std::array<last_state, 4> m_last{};
};

// Update the running game's mice, if it has any; retro_run, after polling
void libretro_mouse_update();
