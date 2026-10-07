#pragma once

// RPCS3's game patches as core options: the Patch Manager category. See
// libretro_patch_manager.cpp.

#include "libretro.h"

#include <functional>
#include <string>
#include <vector>

namespace libretro_patches
{
	using option_getter = std::function<std::string(const char* key)>;

	// The patches for one game: what patch.yml, imported_patch.yml and the
	// game's own <serial>_patch.yml have for its serial. Empty clears them.
	void collect(const std::string& serial);

	// Whether the collected game has any patches (and so the category)
	bool any();

	// The options for the collected patches, to be declared after the fixed
	// ones. The pointers stay valid until the next collect().
	const std::vector<retro_core_option_v2_definition>& definitions();

	// Shows the value choices of a patch only while the patch is on
	void update_display(retro_environment_t cb, const option_getter& get);

	// Writes patch_config.yml for what the options turn on. RPCS3 reads it when
	// the game boots, so a change applies at the next start.
	void write_config(const option_getter& get);
}
