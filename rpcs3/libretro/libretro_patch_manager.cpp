// RPCS3's game patches as core options, the way the cemu core lists a title's
// graphic packs: once the game is known, the patches for its serial become
// options of their own in a last category, Patch Manager, which is not there
// for a game without any (NNshi).
//
// - Every patch is an Enabled/Disabled switch, off unless turned on.
// - Patches that share a Group are one option, a choice between them and
//   Disabled: RPCS3 applies one patch of a group at most.
// - A patch's configurable values with a list of allowed values are options
//   too, shown while the patch is on. Ranges keep the patch's default.
// - "All titles" patches (serial "All", hashed SPU programs) are left out:
//   which ones apply is known only once the game has run its SPU code, and
//   listed for every game they made the category appear everywhere.
//
// The patches come from patch.yml (the one the core carries), from
// imported_patch.yml (what was imported in standalone RPCS3, copied over) and
// from the game's own <serial>_patch.yml, the three RPCS3 loads itself. Which
// are on is the .opt file's business: before each boot patch_config.yml is
// written from the options, and RPCS3 applies a patch only where the hash of
// the executable matches, so the version of the game needs no matching here.

#include "stdafx.h"
#include "libretro_patch_manager.h"

#include "Utilities/bin_patch.h"
#include "Utilities/File.h"

#include <deque>
#include <map>
#include <set>

LOG_CHANNEL(lr_patch_log, "libretro patches");

namespace
{
    struct config_value_option
    {
        std::string key;
        std::string name; // the configurable value's name in the patch
        std::vector<std::pair<std::string /*label*/, f64>> values;
    };

    struct patch_option
    {
        std::string key;
        std::string group;                     // empty: an Enabled/Disabled switch
        std::vector<std::string> descriptions; // one, or a group's patches
        std::map<std::string /*description*/, std::vector<config_value_option>> config_values;
    };

    std::string s_serial;
    patch_engine::patch_map s_map;
    std::vector<patch_option> s_options;
    std::vector<retro_core_option_v2_definition> s_definitions;
    std::deque<std::string> s_strings;

    const char* keep(std::string value)
    {
        s_strings.push_back(std::move(value));
        return s_strings.back().c_str();
    }

    // Short and stable: the same patch of the same game gets the same key in
    // every build, so a choice in the .opt file stays with it.
    std::string key_hash(std::string_view text)
    {
        u32 hash = 0x811c9dc5;
        for (char c : text)
        {
            hash ^= static_cast<u8>(c);
            hash *= 0x01000193;
        }
        return fmt::format("%08x", hash);
    }

    bool for_serial(const patch_engine::patch_info& info, const std::string& serial)
    {
        for (const auto& [title, serials] : info.titles)
        {
            if (serials.contains(serial))
                return true;
        }
        return false;
    }

    // The notes' first paragraph, as the sublabel
    std::string first_paragraph(const std::string& notes)
    {
        std::string text = notes.substr(0, notes.find("\n\n"));
        for (char& c : text)
        {
            if (c == '\n' || c == '\r')
                c = ' ';
        }
        return text;
    }

    std::string value_text(f64 value)
    {
        return fmt::format("%g", value);
    }
}

namespace libretro_patches
{
    void collect(const std::string& serial)
    {
        s_serial = serial;
        s_map.clear();
        s_options.clear();
        s_definitions.clear();
        s_strings.clear();

        if (serial.empty())
            return;

        const std::string dir = patch_engine::get_patches_path();
        for (const std::string& path : {dir + "patch.yml", patch_engine::get_imported_patch_path(), dir + serial + "_patch.yml"})
        {
            if (fs::is_file(path))
                patch_engine::load(s_map, path);
        }

        // One option per description, whichever executables (hashes) it is
        // for; a group's patches together under the group's name
        std::map<std::string, patch_option> by_group;
        std::map<std::string, patch_option> singles;
        std::map<std::string, const patch_engine::patch_info*> first_info;

        for (const auto& [hash, container] : s_map)
        {
            for (const auto& [description, info] : container.patch_info_map)
            {
                if (!for_serial(info, serial))
                    continue;

                first_info.try_emplace(description, &info);

                patch_option& option = info.patch_group.empty() ? singles[description] : by_group[info.patch_group];
                option.group = info.patch_group;
                if (std::find(option.descriptions.begin(), option.descriptions.end(), description) == option.descriptions.end())
                    option.descriptions.push_back(description);
            }
        }

        for (auto* options : {&singles, &by_group})
        {
            for (auto& [name, option] : *options)
            {
                option.key = "rpcs3_zpatch_" + key_hash(serial + '\x1f' + (option.group.empty() ? "p" : "g") + name);
                std::sort(option.descriptions.begin(), option.descriptions.end());

                for (const std::string& description : option.descriptions)
                {
                    const patch_engine::patch_info& info = *first_info.at(description);
                    for (const auto& [value_name, value] : info.default_config_values)
                    {
                        if (value.allowed_values.empty())
                            continue; // a range: the patch's default

                        config_value_option config{};
                        config.key = option.key + "_" + key_hash(description + '\x1f' + value_name);
                        config.name = value_name;
                        for (const auto& allowed : value.allowed_values)
                            config.values.emplace_back(allowed.label.empty() ? value_text(allowed.value) : allowed.label, allowed.value);
                        option.config_values[description].push_back(std::move(config));
                    }
                }
                s_options.push_back(std::move(option));
            }
        }

        // The definitions, in the order the options were made
        for (const patch_option& option : s_options)
        {
            retro_core_option_v2_definition def{};
            def.key = keep(option.key);
            def.category_key = "patches";

            const patch_engine::patch_info& first = *first_info.at(option.descriptions.front());
            if (option.group.empty())
            {
                std::string info = first_paragraph(first.notes);
                if (!first.author.empty())
                    info += (info.empty() ? "" : " ") + fmt::format("(%s)", first.author);
                def.desc = keep(option.descriptions.front());
                def.info = keep(info.empty() ? "Applies at the next start of the game." : info + " Applies at the next start of the game.");
                def.values[0] = {"disabled", nullptr};
                def.values[1] = {"enabled", nullptr};
                def.default_value = "disabled";
            }
            else
            {
                def.desc = keep(option.group);
                def.info = "Patches of which one at most can be on. Applies at the next start of the game.";
                usz index = 0;
                def.values[index++] = {"disabled", nullptr};
                for (const std::string& description : option.descriptions)
                {
                    if (index + 1 >= RETRO_NUM_CORE_OPTION_VALUES_MAX)
                        break;
                    def.values[index++] = {keep(description), nullptr};
                }
                def.default_value = "disabled";
            }
            s_definitions.push_back(def);

            for (const auto& [description, configs] : option.config_values)
            {
                const patch_engine::patch_info& info = *first_info.at(description);
                for (const config_value_option& config : configs)
                {
                    retro_core_option_v2_definition value_def{};
                    value_def.key = keep(config.key);
                    value_def.category_key = "patches";
                    value_def.desc = keep(option.group.empty() ? fmt::format("%s: %s", description, config.name) : fmt::format("%s - %s: %s", option.group, description, config.name));
                    value_def.info = keep(fmt::format("A value of the patch \"%s\".", description));

                    const f64 default_value = ::at32(info.default_config_values, config.name).value;
                    usz index = 0;
                    for (const auto& [label, value] : config.values)
                    {
                        if (index + 1 >= RETRO_NUM_CORE_OPTION_VALUES_MAX)
                            break;
                        value_def.values[index].value = keep(label);
                        if (value == default_value || !value_def.default_value)
                            value_def.default_value = value_def.values[index].value;
                        index++;
                    }
                    s_definitions.push_back(value_def);
                }
            }
        }

        lr_patch_log.notice("%u patch options for %s", s_options.size(), serial);
    }

    bool any()
    {
        return !s_options.empty();
    }

    const std::vector<retro_core_option_v2_definition>& definitions()
    {
        return s_definitions;
    }

    // The patch an option turns on: its description, or empty for none
    static std::string chosen(const patch_option& option, const option_getter& get)
    {
        const std::string value = get(option.key.c_str());
        if (option.group.empty())
            return value == "enabled" ? option.descriptions.front() : std::string{};
        if (std::find(option.descriptions.begin(), option.descriptions.end(), value) != option.descriptions.end())
            return value;
        return {};
    }

    void update_display(retro_environment_t cb, const option_getter& get)
    {
        if (!cb)
            return;
        for (const patch_option& option : s_options)
        {
            const std::string on = chosen(option, get);
            for (const auto& [description, configs] : option.config_values)
            {
                for (const config_value_option& config : configs)
                {
                    retro_core_option_display display{config.key.c_str(), description == on};
                    cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &display);
                }
            }
        }
    }

    void write_config(const option_getter& get)
    {
        // The whole file from the options: what standalone or an earlier game
        // left in it would otherwise stay on
        patch_engine::patch_map config;

        for (const patch_option& option : s_options)
        {
            const std::string on = chosen(option, get);
            if (on.empty())
                continue;

            // The values picked, by name
            std::map<std::string, f64> picked;
            if (const auto it = option.config_values.find(on); it != option.config_values.end())
            {
                for (const config_value_option& value : it->second)
                {
                    const std::string label = get(value.key.c_str());
                    for (const auto& [value_label, number] : value.values)
                    {
                        if (value_label == label)
                            picked[value.name] = number;
                    }
                }
            }

            for (const auto& [hash, container] : s_map)
            {
                const auto info = container.patch_info_map.find(on);
                if (info == container.patch_info_map.end())
                    continue;

                patch_engine::patch_info& out = config[hash].patch_info_map[on];
                out.default_config_values = info->second.default_config_values;
                for (const auto& [title, serials] : info->second.titles)
                {
                    const auto serial = serials.find(s_serial);
                    if (serial == serials.end())
                        continue;
                    for (const auto& [app_version, values] : serial->second)
                    {
                        patch_engine::patch_config_values& out_values = out.titles[title][s_serial][app_version];
                        out_values.enabled = true;
                        out_values.config_values = info->second.default_config_values;
                        for (const auto& [name, number] : picked)
                            out_values.config_values[name].value = number;
                    }
                }
            }
        }

        patch_engine::save_config(config);
    }
}
