#ifndef LIBRETRO_CORE_OPTIONS_H__
#define LIBRETRO_CORE_OPTIONS_H__

#include <stdlib.h>
#include <string.h>

#include "libretro.h"

#ifndef HAVE_NO_LANGEXTRA
#include "libretro_core_options_intl.h"
#endif

/*
 ********************************
 * VERSION: 2.0
 ********************************
 *
 * - 2.0: Add support for core options v2 interface
 * - 1.3: Move translations to libretro_core_options_intl.h
 *        - libretro_core_options_intl.h includes BOM and utf-8
 *          fix for MSVC 2010-2013
 *        - Added HAVE_NO_LANGEXTRA flag to disable translations
 *          on platforms/compilers without BOM support
 * - 1.2: Use core options v1 interface when
 *        RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION is >= 1
 *        (previously required RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION == 1)
 * - 1.1: Support generation of core options v0 retro_core_option_value
 *        arrays containing options with a single value
 * - 1.0: First commit
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 ********************************
 * Core Option Definitions
 ********************************
*/

/* RETRO_LANGUAGE_ENGLISH */

/* Default language:
 * - All other languages must include the same keys and values
 * - Will be used as a fallback in the event that frontend language
 *   is not available
 * - Will be used as a fallback for any missing entries in
 *   frontend language definition
 * - Translations live in libretro_core_options_intl.h, which is
 *   generated from Crowdin - edit only the English texts here
 */

struct retro_core_option_v2_category option_cats_us[] = {
    { "cpu", "CPU", "PPU/SPU decoder, threads, cache, and CPU emulation options." },
    { "gpu", "GPU", "Renderer, resolution, shaders, and graphics options." },
    { "audio", "Audio", "Audio buffering, volume, and microphone options." },
    { "network", "Network", "Network, PSN, RPCN, and online options." },
    { "advanced", "Advanced", "Advanced accuracy and performance tuning options." },
    { "core", "Core", "System language, region, and misc core options." },
    { NULL, NULL, NULL }
};

struct retro_core_option_v2_definition option_defs_us[] = {
    // ==================== CPU OPTIONS ====================
    {
        "rpcs3_ppu_decoder", "PPU Decoder", NULL,
        "PPU (main CPU) decoder. LLVM Recompiler is fastest.",
        NULL, "cpu",
        { {"llvm", "Recompiler (LLVM)"}, {"interpreter", "Interpreter (Slow)"}, {NULL, NULL} },
        "llvm"
    },
    {
        "rpcs3_spu_decoder", "SPU Decoder", NULL,
        "SPU (co-processor) decoder. LLVM Recompiler is fastest.",
        NULL, "cpu",
        { {"llvm", "Recompiler (LLVM)"}, {"asmjit", "Recompiler (ASMJIT)"}, {"interpreter", "Interpreter (Slow)"}, {NULL, NULL} },
        "llvm"
    },
    {
        "rpcs3_spu_block_size", "SPU Block Size", NULL,
        "SPU recompiler block size. Mega/Giga may improve performance.",
        NULL, "cpu",
        { {"safe", "Safe"}, {"mega", "Mega"}, {"giga", "Giga"}, {NULL, NULL} },
        "safe"
    },
    {
        "rpcs3_preferred_spu_threads", "Preferred SPU Threads", NULL,
        "Number of SPU threads. Auto recommended.",
        NULL, "cpu",
        { {"0", "Auto"}, {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"}, {"5", "5"}, {"6", "6"}, {NULL, NULL} },
        "0"
    },
    {
        "rpcs3_spu_loop_detection", "SPU Loop Detection", NULL,
        "Enable SPU loop detection for performance.",
        NULL, "cpu",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_spu_cache", "SPU Cache", NULL,
        "Enable SPU cache for faster subsequent loads.",
        NULL, "cpu",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_llvm_precompilation", "LLVM Precompilation", NULL,
        "Precompile PPU modules at boot for faster subsequent loads.",
        NULL, "cpu",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_accurate_dfma", "Accurate DFMA", NULL,
        "Use accurate double-precision fused multiply-add.",
        NULL, "cpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_ppu_reservations", "PPU Thread Reservations", NULL,
        "Use PPU thread reservations for accurate locking.",
        NULL, "cpu",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_accurate_xfloat", "Accurate XFLOAT", NULL,
        "More accurate SPU floating-point. May fix some games.",
        NULL, "cpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_clocks_scale", "Clocks Scale", NULL,
        "Scale PS3 clock speed percentage.",
        NULL, "cpu",
        { {"50", "50%"}, {"75", "75%"}, {"100", "100%"}, {"150", "150%"}, {"200", "200%"}, {"300", "300%"}, {NULL, NULL} },
        "100"
    },
    {
        "rpcs3_sleep_timers_accuracy", "Sleep Timers Accuracy", NULL,
        "Sleep timers accuracy level.",
        NULL, "cpu",
        { {"usleep", "Usleep"}, {"all_timers", "All Timers"}, {"as_host", "As Host"}, {NULL, NULL} },
        "usleep"
    },
    {
        "rpcs3_max_spurs_threads", "Max SPURS Threads", NULL,
        "Maximum SPURS thread count. Lower may improve performance.",
        NULL, "cpu",
        { {"auto", "Auto"}, {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"}, {"5", "5"}, {"6", "6"}, {NULL, NULL} },
        "auto"
    },
    {
        "rpcs3_enable_tsx", "Enable TSX", NULL,
        "Enable Intel TSX hardware acceleration if available.",
        NULL, "cpu",
        { {"enabled", NULL}, {"disabled", NULL}, {"forced", "Forced"}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_spu_xfloat_accuracy", "SPU XFloat Accuracy", NULL,
        "SPU floating-point accuracy level.",
        NULL, "cpu",
        { {"relaxed", "Relaxed (Fastest)"}, {"accurate", "Accurate"}, {"ultra", "Ultra (Slowest)"}, {NULL, NULL} },
        "accurate"
    },
    {
        "rpcs3_spu_dma_busy_wait", "SPU DMA Busy Waiting", NULL,
        "Enable SPU DMA busy waiting for timing accuracy.",
        NULL, "cpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_ppu_llvm_java_mode", "PPU LLVM Java Mode Handling", NULL,
        "PPU LLVM Java mode compliance level.",
        NULL, "cpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },

    // ==================== GPU OPTIONS ====================
    {
        "rpcs3_renderer", "Renderer", NULL,
        "Vulkan finishes each frame into memory and the frontend takes the pixels; it runs about as fast as standalone RPCS3 and works where OpenGL is missing or broken. OpenGL draws straight into the frontend's context.",
        NULL, "gpu",
        { {"opengl", "OpenGL"}, {"vulkan", "Vulkan (through memory)"}, {"null", "Null (No Video)"}, {NULL, NULL} },
        "vulkan"
    },
    {
        "rpcs3_resolution_scale", "Resolution Scale", NULL,
        "Internal rendering resolution scale percentage.",
        NULL, "gpu",
        { {"25", "25%"}, {"30", "30%"}, {"35", "35%"}, {"40", "40%"}, {"45", "45%"}, {"50", "50%"}, {"55", "55%"}, {"60", "60%"}, {"65", "65%"}, {"70", "70%"}, {"75", "75%"}, {"80", "80%"}, {"85", "85%"}, {"90", "90%"}, {"95", "95%"}, {"100", "100% (Native)"}, {"105", "105%"}, {"110", "110%"}, {"115", "115%"}, {"120", "120%"}, {"125", "125%"}, {"130", "130%"}, {"135", "135%"}, {"140", "140%"}, {"145", "145%"}, {"150", "150%"}, {"175", "175%"}, {"200", "200%"}, {"250", "250%"}, {"300", "300%"}, {NULL, NULL} },
        "100"
    },
    {
        "rpcs3_frame_limit", "Frame Limit", NULL,
        "Limit frame rate. Auto uses RetroArch timing.",
        NULL, "gpu",
        { {"auto", "Auto"}, {"off", "Off"}, {"30", "30 FPS"}, {"50", "50 FPS"}, {"60", "60 FPS"}, {"120", "120 FPS"}, {"144", "144 FPS"}, {"240", "240 FPS"}, {NULL, NULL} },
        "auto"
    },
    {
        "rpcs3_shader_mode", "Shader Mode", NULL,
        "How shaders are compiled. Async compiles in the background but the frame that first needs a pipeline waits for it, which is what a long freeze on a new scene usually is. Async with Shader Interpreter draws that frame through the interpreter instead and swaps in the compiled shader when it is ready: no freeze, lower speed while it catches up.",
        NULL, "gpu",
        { {"async", "Async (Recommended)"}, {"async_interpreter", "Async with Shader Interpreter (no stalls)"}, {"async_recompiler", "Async with Recompiler"}, {"interpreter", "Shader Interpreter only"}, {"sync", "Synchronous"}, {NULL, NULL} },
        "async"
    },
    {
        "rpcs3_shader_compiler_threads", "Shader Compiler Threads", NULL,
        "How many threads compile shaders. Auto lets RPCS3 decide from the CPU it sees.",
        NULL, "gpu",
        { {"auto", "Auto"}, {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"}, {"6", "6"}, {"8", "8"}, {NULL, NULL} },
        "auto"
    },
    {
        "rpcs3_anisotropic_filter", "Anisotropic Filtering", NULL,
        "Texture filtering quality.",
        NULL, "gpu",
        { {"auto", "Auto"}, {"1", "1x (Off)"}, {"2", "2x"}, {"4", "4x"}, {"8", "8x"}, {"16", "16x"}, {NULL, NULL} },
        "auto"
    },
    {
        "rpcs3_msaa", "Anti-Aliasing (MSAA)", NULL,
        "Multi-sample anti-aliasing.",
        NULL, "gpu",
        { {"disabled", NULL}, {"2", "2x"}, {"4", "4x"}, {"8", "8x"}, {"16", "16x"}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_shader_precision", "Shader Precision", NULL,
        "Shader floating-point precision.",
        NULL, "gpu",
        { {"low", "Low (Fastest)"}, {"normal", "Normal"}, {"high", "High (Most Accurate)"}, {NULL, NULL} },
        "normal"
    },
    {
        "rpcs3_write_color_buffers", "Write Color Buffers", NULL,
        "Write color buffers to main memory. Fixes some effects.",
        NULL, "gpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_read_color_buffers", "Read Color Buffers", NULL,
        "Read color buffers from main memory.",
        NULL, "gpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_read_depth_buffers", "Read Depth Buffers", NULL,
        "Read depth buffers from main memory.",
        NULL, "gpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_write_depth_buffers", "Write Depth Buffers", NULL,
        "Write depth buffers to main memory.",
        NULL, "gpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_strict_rendering", "Strict Rendering Mode", NULL,
        "Enable strict rendering for accuracy.",
        NULL, "gpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_vertex_cache", "Vertex Cache", NULL,
        "Enable vertex cache for performance.",
        NULL, "gpu",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_multithreaded_rsx", "Multithreaded RSX", NULL,
        "Enable multithreaded RSX for better performance.",
        NULL, "gpu",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_zcull_accuracy", "ZCULL Accuracy", NULL,
        "ZCULL occlusion query accuracy.",
        NULL, "gpu",
        { {"relaxed", "Relaxed (Fastest)"}, {"approximate", "Approximate"}, {"precise", "Precise (Slowest)"}, {NULL, NULL} },
        "relaxed"
    },
    {
        "rpcs3_cpu_blit", "Force CPU Blit", NULL,
        "Force CPU blit emulation for certain effects.",
        NULL, "gpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_driver_wakeup_delay", "Driver Wake-Up Delay", NULL,
        "Driver wake-up delay in microseconds.",
        NULL, "gpu",
        { {"0", "0 (Minimum)"}, {"20", "20"}, {"50", "50"}, {"100", "100"}, {"200", "200 (Default)"}, {"400", "400"}, {"800", "800"}, {NULL, NULL} },
        "200"
    },
    {
        "rpcs3_vblank_rate", "VBlank Rate", NULL,
        "VBlank frequency in Hz.",
        NULL, "gpu",
        { {"50", "50 Hz (PAL)"}, {"60", "60 Hz (NTSC)"}, {"120", "120 Hz"}, {"144", "144 Hz"}, {"240", "240 Hz"}, {NULL, NULL} },
        "60"
    },
    {
        "rpcs3_stretch_to_display", "Stretch to Display", NULL,
        "Stretch game output to fill the display.",
        NULL, "gpu",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },

    // ==================== AUDIO OPTIONS ====================
    {
        "rpcs3_audio_buffering", "Enable Buffering", NULL,
        "Enable audio buffering.",
        NULL, "audio",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_audio_buffer_duration", "Buffer Duration", NULL,
        "Audio buffer size in milliseconds.",
        NULL, "audio",
        { {"10", "10ms"}, {"20", "20ms"}, {"30", "30ms"}, {"40", "40ms"}, {"50", "50ms"}, {"75", "75ms"}, {"100", "100ms (Default)"}, {"150", "150ms"}, {"200", "200ms"}, {NULL, NULL} },
        "100"
    },
    {
        "rpcs3_time_stretching", "Time Stretching", NULL,
        "Enable audio time stretching to reduce stuttering.",
        NULL, "audio",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_microphone_type", "Microphone Type", NULL,
        "Microphone device type.",
        NULL, "audio",
        { {"null", "Null (Disabled)"}, {"standard", "Standard"}, {"singstar", "SingStar"}, {"real_singstar", "Real SingStar"}, {"rocksmith", "Rocksmith"}, {NULL, NULL} },
        "null"
    },
    {
        "rpcs3_master_volume", "Master Volume", NULL,
        "Master audio volume percentage.",
        NULL, "audio",
        { {"0", "0%"}, {"10", "10%"}, {"20", "20%"}, {"30", "30%"}, {"40", "40%"}, {"50", "50%"}, {"60", "60%"}, {"70", "70%"}, {"80", "80%"}, {"90", "90%"}, {"100", "100%"}, {NULL, NULL} },
        "100"
    },

    // ==================== NETWORK OPTIONS ====================
    {
        "rpcs3_network_enabled", "Network Enabled", NULL,
        "Enable network features.",
        NULL, "network",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_psn_status", "PSN Status", NULL,
        "PlayStation Network status.",
        NULL, "network",
        { {"disabled", NULL}, {"simulated", "Simulated"}, {"rpcn", "RPCN"}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_upnp", "UPNP", NULL,
        "Enable UPNP for automatic port forwarding.",
        NULL, "network",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_show_rpcn_popups", "Show RPCN Popups", NULL,
        "Show RPCN notification popups.",
        NULL, "network",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_show_trophy_popups", "Show Trophy Popups", NULL,
        "Show trophy unlock notifications.",
        NULL, "network",
        { {"enabled", NULL}, {"disabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_dns", "DNS Server", NULL,
        "DNS server address.",
        NULL, "network",
        { {"8.8.8.8", "Google DNS"}, {"1.1.1.1", "Cloudflare DNS"}, {"208.67.222.222", "OpenDNS"}, {NULL, NULL} },
        "8.8.8.8"
    },
    {
        "rpcs3_rpcn_server", "RPCN Server", NULL,
        "RPCN server address for online play.",
        NULL, "network",
        { {"rpcn.rpcs3.net", "Official RPCN"}, {"custom", "Custom"}, {NULL, NULL} },
        "rpcn.rpcs3.net"
    },

    // ==================== ADVANCED OPTIONS ====================
    {
        "rpcs3_spu_verification", "SPU Verification", NULL,
        "SPU code verification level.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "enabled"
    },
    {
        "rpcs3_spu_cache_line_stores", "SPU Cache Line Stores", NULL,
        "Enable accurate cache line stores.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_rsx_fifo_accuracy", "RSX FIFO Accuracy", NULL,
        "RSX FIFO command accuracy level.",
        NULL, "advanced",
        { {"fast", "Fast"}, {"balanced", "Balanced"}, {"accurate", "Accurate"}, {NULL, NULL} },
        "fast"
    },
    {
        "rpcs3_driver_recovery_timeout", "Driver Recovery Timeout", NULL,
        "GPU driver recovery timeout in milliseconds.",
        NULL, "advanced",
        { {"0", "Disabled"}, {"1000", "1 second"}, {"2000", "2 seconds"}, {"5000", "5 seconds"}, {"10000", "10 seconds"}, {NULL, NULL} },
        "1000"
    },
    {
        "rpcs3_mfc_shuffling", "MFC Commands Shuffling", NULL,
        "Shuffle MFC commands for accuracy.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_spu_delay_penalty", "SPU Delay Penalty", NULL,
        "SPU delay penalty for scheduling.",
        NULL, "advanced",
        { {"0", "0"}, {"1", "1"}, {"2", "2"}, {"3", "3 (Default)"}, {"4", "4"}, {"5", "5"}, {NULL, NULL} },
        "3"
    },
    {
        "rpcs3_zcull_sync", "Relaxed ZCull Sync", NULL,
        "Use relaxed ZCull synchronization.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_async_texture_streaming", "Async Texture Streaming", NULL,
        "Enable asynchronous texture streaming.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_ppu_llvm_greedy", "PPU LLVM Greedy Mode", NULL,
        "Use greedy PPU LLVM compilation.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_spu_nj_fixup", "SPU NJ Fixup", NULL,
        "Apply SPU non-Java mode fixup.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_ppu_nj_mode", "PPU NJ Fixup Mode", NULL,
        "PPU non-Java mode handling.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_ppu_set_sat_bit", "Set Saturation Bit", NULL,
        "Accurately set PPU saturation bit.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_ppu_accurate_vector_nan", "PPU Accurate Vector NaN", NULL,
        "More accurate vector NaN handling.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_ppu_set_fpcc", "PPU Set FPCC", NULL,
        "Accurately set PPU FPCC bits.",
        NULL, "advanced",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },

    // ==================== CORE OPTIONS ====================
    {
        "rpcs3_language", "System Language", NULL,
        "PS3 system language.",
        NULL, "core",
        { {"english", "English"}, {"japanese", "Japanese"}, {"french", "French"}, {"spanish", "Spanish"}, {"german", "German"}, {"italian", "Italian"}, {"dutch", "Dutch"}, {"portuguese", "Portuguese"}, {"russian", "Russian"}, {"korean", "Korean"}, {"chinese_trad", "Chinese (Traditional)"}, {"chinese_simp", "Chinese (Simplified)"}, {NULL, NULL} },
        "english"
    },
    {
        "rpcs3_enter_button", "Confirm Button", NULL,
        "Button used for confirm actions.",
        NULL, "core",
        { {"cross", "Cross (Western)"}, {"circle", "Circle (Japanese)"}, {NULL, NULL} },
        "cross"
    },
    {
        "rpcs3_license_area", "License Area", NULL,
        "PS3 license region.",
        NULL, "core",
        { {"usa", "USA"}, {"eu", "Europe"}, {"jp", "Japan"}, {"hk", "Hong Kong"}, {"kr", "Korea"}, {NULL, NULL} },
        "usa"
    },
    {
        "rpcs3_show_shader_compilation_hint", "Show Shader Compilation Hint", NULL,
        "Show hint when shaders are being compiled.",
        NULL, "core",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_show_ppu_compilation_hint", "Show PPU Compilation Hint", NULL,
        "Show hint when PPU modules are being compiled.",
        NULL, "core",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_vfs_init", "VFS Initialize Mode", NULL,
        "Virtual file system initialization mode.",
        NULL, "core",
        { {"auto", "Auto"}, {"reset", "Reset"}, {NULL, NULL} },
        "auto"
    },
    {
        "rpcs3_silence_all_logs", "Silence All Logs", NULL,
        "Silence all log output for performance.",
        NULL, "core",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_hook_static_funcs", "Hook Static Functions", NULL,
        "Hook static functions for HLE.",
        NULL, "core",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },
    {
        "rpcs3_hle_lwmutex", "HLE lwmutex", NULL,
        "Use HLE implementation for lwmutex.",
        NULL, "core",
        { {"disabled", NULL}, {"enabled", NULL}, {NULL, NULL} },
        "disabled"
    },

    { NULL, NULL, NULL, NULL, NULL, NULL, {{NULL, NULL}}, NULL }
};

struct retro_core_options_v2 options_us = {
   option_cats_us,
   option_defs_us
};

/*
 ********************************
 * Language Mapping
 ********************************
*/

#ifndef HAVE_NO_LANGEXTRA
struct retro_core_options_v2 *options_intl[RETRO_LANGUAGE_LAST] = {
   &options_us,       /* RETRO_LANGUAGE_ENGLISH */
   &options_ja,       /* RETRO_LANGUAGE_JAPANESE */
   &options_fr,       /* RETRO_LANGUAGE_FRENCH */
   &options_es,       /* RETRO_LANGUAGE_SPANISH */
   &options_de,       /* RETRO_LANGUAGE_GERMAN */
   &options_it,       /* RETRO_LANGUAGE_ITALIAN */
   &options_nl,       /* RETRO_LANGUAGE_DUTCH */
   &options_pt_br,    /* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */
   &options_pt_pt,    /* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */
   &options_ru,       /* RETRO_LANGUAGE_RUSSIAN */
   &options_ko,       /* RETRO_LANGUAGE_KOREAN */
   &options_cht,      /* RETRO_LANGUAGE_CHINESE_TRADITIONAL */
   &options_chs,      /* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */
   &options_eo,       /* RETRO_LANGUAGE_ESPERANTO */
   &options_pl,       /* RETRO_LANGUAGE_POLISH */
   &options_vn,       /* RETRO_LANGUAGE_VIETNAMESE */
   &options_ar,       /* RETRO_LANGUAGE_ARABIC */
   &options_el,       /* RETRO_LANGUAGE_GREEK */
   &options_tr,       /* RETRO_LANGUAGE_TURKISH */
   &options_sk,       /* RETRO_LANGUAGE_SLOVAK */
   &options_fa,       /* RETRO_LANGUAGE_PERSIAN */
   &options_he,       /* RETRO_LANGUAGE_HEBREW */
   &options_ast,      /* RETRO_LANGUAGE_ASTURIAN */
   &options_fi,       /* RETRO_LANGUAGE_FINNISH */
   &options_id,       /* RETRO_LANGUAGE_INDONESIAN */
   &options_sv,       /* RETRO_LANGUAGE_SWEDISH */
   &options_uk,       /* RETRO_LANGUAGE_UKRAINIAN */
   &options_cs,       /* RETRO_LANGUAGE_CZECH */
   &options_val,      /* RETRO_LANGUAGE_CATALAN_VALENCIA */
   &options_ca,       /* RETRO_LANGUAGE_CATALAN */
   &options_en,       /* RETRO_LANGUAGE_BRITISH_ENGLISH */
   &options_hu,       /* RETRO_LANGUAGE_HUNGARIAN */
   &options_be,       /* RETRO_LANGUAGE_BELARUSIAN */
   &options_gl,       /* RETRO_LANGUAGE_GALICIAN */
   &options_no,       /* RETRO_LANGUAGE_NORWEGIAN */
   &options_ga,       /* RETRO_LANGUAGE_IRISH */
};
#endif

/*
 ********************************
 * Functions
 ********************************
*/

/* Handles configuration/setting of core options.
 * Should be called as early as possible - ideally inside
 * retro_set_environment(), and no later than retro_load_game()
 * > We place the function body in the header to avoid the
 *   necessity of adding more .c files (i.e. want this to
 *   be as painless as possible for core devs)
 */

static inline void libretro_set_core_options(retro_environment_t environ_cb,
      bool *categories_supported)
{
   unsigned version  = 0;
#ifndef HAVE_NO_LANGEXTRA
   unsigned language = 0;
#endif

   if (!environ_cb || !categories_supported)
      return;

   *categories_supported = false;

   if (!environ_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version))
      version = 0;

   if (version >= 2)
   {
#ifndef HAVE_NO_LANGEXTRA
      struct retro_core_options_v2_intl core_options_intl;

      core_options_intl.us    = &options_us;
      core_options_intl.local = NULL;

      if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
          (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH))
         core_options_intl.local = options_intl[language];

      *categories_supported = environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL,
            &core_options_intl);
#else
      *categories_supported = environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2,
            &options_us);
#endif
   }
   else
   {
      size_t i, j;
      size_t option_index              = 0;
      size_t num_options               = 0;
      struct retro_core_option_definition
            *option_v1_defs_us         = NULL;
#ifndef HAVE_NO_LANGEXTRA
      size_t num_options_intl          = 0;
      struct retro_core_option_v2_definition
            *option_defs_intl          = NULL;
      struct retro_core_option_definition
            *option_v1_defs_intl       = NULL;
      struct retro_core_options_intl
            core_options_v1_intl;
#endif
      struct retro_variable *variables = NULL;
      char **values_buf                = NULL;

      /* Determine total number of options */
      while (true)
      {
         if (option_defs_us[num_options].key)
            num_options++;
         else
            break;
      }

      if (version >= 1)
      {
         /* Allocate US array */
         option_v1_defs_us = (struct retro_core_option_definition *)
               calloc(num_options + 1, sizeof(struct retro_core_option_definition));

         /* Copy parameters from option_defs_us array */
         for (i = 0; i < num_options; i++)
         {
            struct retro_core_option_v2_definition *option_def_us = &option_defs_us[i];
            struct retro_core_option_value *option_values         = option_def_us->values;
            struct retro_core_option_definition *option_v1_def_us = &option_v1_defs_us[i];
            struct retro_core_option_value *option_v1_values      = option_v1_def_us->values;

            option_v1_def_us->key           = option_def_us->key;
            option_v1_def_us->desc          = option_def_us->desc;
            option_v1_def_us->info          = option_def_us->info;
            option_v1_def_us->default_value = option_def_us->default_value;

            /* Values must be copied individually... */
            while (option_values->value)
            {
               option_v1_values->value = option_values->value;
               option_v1_values->label = option_values->label;

               option_values++;
               option_v1_values++;
            }
         }

#ifndef HAVE_NO_LANGEXTRA
         if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
             (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH) &&
             options_intl[language])
            option_defs_intl = options_intl[language]->definitions;

         if (option_defs_intl)
         {
            /* Determine number of intl options */
            while (true)
            {
               if (option_defs_intl[num_options_intl].key)
                  num_options_intl++;
               else
                  break;
            }

            /* Allocate intl array */
            option_v1_defs_intl = (struct retro_core_option_definition *)
                  calloc(num_options_intl + 1, sizeof(struct retro_core_option_definition));

            /* Copy parameters from option_defs_intl array */
            for (i = 0; i < num_options_intl; i++)
            {
               struct retro_core_option_v2_definition *option_def_intl = &option_defs_intl[i];
               struct retro_core_option_value *option_values           = option_def_intl->values;
               struct retro_core_option_definition *option_v1_def_intl = &option_v1_defs_intl[i];
               struct retro_core_option_value *option_v1_values        = option_v1_def_intl->values;

               option_v1_def_intl->key           = option_def_intl->key;
               option_v1_def_intl->desc          = option_def_intl->desc;
               option_v1_def_intl->info          = option_def_intl->info;
               option_v1_def_intl->default_value = option_def_intl->default_value;

               /* Values must be copied individually... */
               while (option_values->value)
               {
                  option_v1_values->value = option_values->value;
                  option_v1_values->label = option_values->label;

                  option_values++;
                  option_v1_values++;
               }
            }
         }

         core_options_v1_intl.us    = option_v1_defs_us;
         core_options_v1_intl.local = option_v1_defs_intl;

         environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL, &core_options_v1_intl);
#else
         environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, option_v1_defs_us);
#endif
      }
      else
      {
         /* Allocate arrays */
         variables  = (struct retro_variable *)calloc(num_options + 1,
               sizeof(struct retro_variable));
         values_buf = (char **)calloc(num_options, sizeof(char *));

         if (!variables || !values_buf)
            goto error;

         /* Copy parameters from option_defs_us array */
         for (i = 0; i < num_options; i++)
         {
            const char *key                        = option_defs_us[i].key;
            const char *desc                       = option_defs_us[i].desc;
            const char *default_value              = option_defs_us[i].default_value;
            struct retro_core_option_value *values = option_defs_us[i].values;
            size_t buf_len                         = 3;
            size_t default_index                   = 0;

            values_buf[i] = NULL;

            if (desc)
            {
               size_t num_values = 0;

               /* Determine number of values */
               while (true)
               {
                  if (values[num_values].value)
                  {
                     /* Check if this is the default value */
                     if (default_value)
                        if (strcmp(values[num_values].value, default_value) == 0)
                           default_index = num_values;

                     buf_len += strlen(values[num_values].value);
                     num_values++;
                  }
                  else
                     break;
               }

               /* Build values string */
               if (num_values > 0)
               {
                  buf_len += num_values - 1;
                  buf_len += strlen(desc);

                  values_buf[i] = (char *)calloc(buf_len, sizeof(char));
                  if (!values_buf[i])
                     goto error;

                  strcpy(values_buf[i], desc);
                  strcat(values_buf[i], "; ");

                  /* Default value goes first */
                  strcat(values_buf[i], values[default_index].value);

                  /* Add remaining values */
                  for (j = 0; j < num_values; j++)
                  {
                     if (j != default_index)
                     {
                        strcat(values_buf[i], "|");
                        strcat(values_buf[i], values[j].value);
                     }
                  }
               }
            }

            variables[option_index].key   = key;
            variables[option_index].value = values_buf[i];
            option_index++;
         }

         /* Set variables */
         environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
      }

error:
      /* Clean up */

      if (option_v1_defs_us)
      {
         free(option_v1_defs_us);
         option_v1_defs_us = NULL;
      }

#ifndef HAVE_NO_LANGEXTRA
      if (option_v1_defs_intl)
      {
         free(option_v1_defs_intl);
         option_v1_defs_intl = NULL;
      }
#endif

      if (values_buf)
      {
         for (i = 0; i < num_options; i++)
         {
            if (values_buf[i])
            {
               free(values_buf[i]);
               values_buf[i] = NULL;
            }
         }

         free(values_buf);
         values_buf = NULL;
      }

      if (variables)
      {
         free(variables);
         variables = NULL;
      }
   }
}

#ifdef __cplusplus
}
#endif

#endif
