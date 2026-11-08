#include "stdafx.h"

#include "libretro_mouse_handler.h"
#include "libretro_input.h"

#include "Emu/System.h"
#include "Emu/IdManager.h"

void LibretroMouseHandler::Init(const u32 max_connect)
{
    // The USIO board calls this on every poll; as upstream's handlers, only
    // the first call sets the mice up. A savestate brings max_connect back
    // before calling this, with no mice yet.
    if (m_info.max_connect > 0 && !m_mice.empty())
        return;

    m_mice.clear();
    m_mice.resize(max_connect);
    m_last = {};

    const u32 connected = std::min<u32>(max_connect, ::size32(m_last));

    m_info = {};
    m_info.max_connect = max_connect;
    m_info.now_connect = connected;
    for (u32 i = 0; i < max_connect; i++)
    {
        m_info.status[i] = i < connected ? CELL_MOUSE_STATUS_CONNECTED : CELL_MOUSE_STATUS_DISCONNECTED;
        m_info.mode[i] = CELL_MOUSE_INFO_TABLET_MOUSE_MODE;
        m_info.tablet_is_supported[i] = CELL_MOUSE_INFO_TABLET_NOT_SUPPORTED;
        m_info.vendor_id[i] = 0x1234;
        m_info.product_id[i] = 0x1234;
    }

    type = mouse_handler::raw;
}

void LibretroMouseHandler::update()
{
    u32 count = 0;
    {
        std::lock_guard lock(mutex);
        count = std::min<u32>(::size32(m_mice), ::size32(m_last));
    }

    // RetroArch's range is -0x7fff..0x7fff; RPCS3 takes 0..max
    constexpr s32 range = 0x7fff * 2;

    for (u32 i = 0; i < count; i++)
    {
        const LibretroGunState& gun = libretro_input_get_gun(libretro_input_port_for_player(i));
        last_state& last = m_last[i];

        // Off the screen the mouse stays where it last was, so a shot fired
        // there (to reload) lands at the edge it left by
        const s32 x = gun.x + 0x7fff;
        const s32 y = gun.y + 0x7fff;
        if (x != last.x || y != last.y)
        {
            Move(i, x, y, range, range);
            last.x = x;
            last.y = y;
        }

        constexpr u8 b1 = static_cast<u8>(CELL_MOUSE_BUTTON_1);
        constexpr u8 b2 = static_cast<u8>(CELL_MOUSE_BUTTON_2);
        constexpr u8 b3 = static_cast<u8>(CELL_MOUSE_BUTTON_3);
        const u8 buttons = static_cast<u8>((gun.trigger ? b1 : 0) | (gun.aux_a ? b2 : 0) | (gun.aux_b ? b3 : 0));
        for (const u8 button : {b1, b2, b3})
        {
            if ((buttons ^ last.buttons) & button)
                Button(i, button, (buttons & button) != 0);
        }
        last.buttons = buttons;

        if (gun.wheel)
            Scroll(i, 0, gun.wheel);
    }
}

void libretro_mouse_update()
{
    if (Emu.IsStopped())
        return;

    if (auto* handler = g_fxo->try_get<MouseHandlerBase>())
    {
        if (auto* mouse = dynamic_cast<LibretroMouseHandler*>(handler))
            mouse->update();
    }
}
