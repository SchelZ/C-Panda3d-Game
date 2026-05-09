#pragma once

#include "load_prc_file.h"
#include "dev_flags.h"
#include "str_encrypt.h"

inline void load_embedded_prc() {
    load_prc_file_data("", ESTR(R"(

# ── Display ──────────────────────────────────────────────
        load-display pandagl
        aux-display pandadx9

        win-size 1280 720
        win-origin -2 -2
        window-title Crystal Collector
        fullscreen #f
        win-fixed-size #f
        undecorated #f
        
        color-bits 24
        alpha-bits 8
        depth-bits 24
        stencil-bits 0
        multisamples 2

        sync-video #t
        clock-mode limited
        clock-frame-rate 60
        sleep-precision 0.01

# ── Models ───────────────────────────────────────────────
        load-file-type egg pandaegg
        model-path $MAIN_DIR/models

# ── Textures ─────────────────────────────────────────────
        texture-anisotropic-degree 4
        compressed-textures #t
        keep-texture-ram #f
        model-cache-textures #t
        max-texture-dimension 0

# ── Performance ──────────────────────────────────────────
        cursor-hidden #f
        interpolate-frames #t
        default-coordinate-system zup-right
        use-scene-graph-reduce #t
        allow-incomplete-render #f
        pipeline-frames 2
        garbage-collect-states #t
        garbage-collect-states-rate 1.0
        flatten-geoms #t

# ── Audio ─────────────────────────────────────────────────
        audio-library-name p3openal_audio
        audio-master-volume 1.0
        audio-music-active #t
        audio-sfx-active #t

# ── Logging ───────────────────────────────────────────────
        notify-level warning
        notify-level-display info
        notify-level-panda silent
        notify-level-loader warning
 
# ── Misc ──────────────────────────────────────────────────
        cursor-hidden #f
        interpolate-frames #t
        default-coordinate-system zup-right
        interpolate-animations #t
        auto-normalize-pending #f

    )"));

    // ── Threading ─────────────────────────────────────────────────
    // DEV_FREEZE_LOADING_SCREEN uses a spin-loop in dismiss() which
    // deadlocks under App/Cull/Draw — keep single-threaded then.
#ifdef DEV_FREEZE_LOADING_SCREEN
    load_prc_file_data("", "threading-model 1/1");
#else
    load_prc_file_data("", "threading-model App/Cull/Draw");
#endif

    // ── Dev overrides ─────────────────────────────────────────────
#ifdef DEV_SHOW_FPS
    load_prc_file_data("", "show-frame-rate-meter #t");
#else
    load_prc_file_data("", "show-frame-rate-meter #f");
#endif

#ifdef DEV_TIMING_STATS
    load_prc_file_data("", "notify-level-display debug");
    load_prc_file_data("", "gl-check-errors #t");
    load_prc_file_data("", "gl-debug #t");
#endif
}