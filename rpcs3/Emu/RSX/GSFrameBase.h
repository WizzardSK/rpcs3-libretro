#pragma once

#include "util/types.hpp"
#include <vector>

#include "display.h"

// Set by the libretro core when it has no hardware context to render into and
// wants finished frames handed back as pixels instead. present_frame() already
// exists for recording and screenshots and does exactly that; this lets the
// renderers take that path every frame without pretending a recording is in
// progress. It costs a readback per frame, which is the price of a frontend
// that cannot be given a GPU image - on Android, where there is no desktop
// OpenGL for the port to ask for, it is that or nothing.
extern bool g_libretro_software_present;

class GSFrameBase
{
public:
	GSFrameBase() = default;
	GSFrameBase(const GSFrameBase&) = delete;
	virtual ~GSFrameBase() = default;

	virtual void close() = 0;
	virtual void reset() = 0;
	virtual bool shown() = 0;
	virtual void hide() = 0;
	virtual void show() = 0;
	virtual void toggle_fullscreen() = 0;

	virtual void delete_context(draw_context_t ctx) = 0;
	virtual draw_context_t make_context() = 0;
	virtual void set_current(draw_context_t ctx) = 0;
	virtual void flip(draw_context_t ctx, bool skip_frame = false) = 0;
	virtual int client_width() = 0;
	virtual int client_height() = 0;
	virtual f64 client_display_rate() = 0;
	virtual bool has_alpha() = 0;

	virtual display_handle_t handle() const = 0;

	virtual bool can_consume_frame() const = 0;
	virtual void present_frame(std::vector<u8>&& data, u32 pitch, u32 width, u32 height, bool is_bgra) const = 0;
	virtual void take_screenshot(std::vector<u8>&& sshot_data, u32 sshot_width, u32 sshot_height, bool is_bgra) = 0;
};
