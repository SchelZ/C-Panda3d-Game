// dev_flags.h — Development / debug feature flags.
//
// Uncomment a flag to enable that debug behaviour.
// ALL flags in this file must be commented out for release builds.
//
// Usage in code:
//   #ifdef DEV_FREEZE_LOADING_SCREEN
//       // loading screen never dismisses
//   #endif
//
// You can also pass flags from CMake/VS without touching this file:
//   CMake:   -DCMAKE_CXX_FLAGS="-DDEV_FREEZE_LOADING_SCREEN"
//   VS2022:  Project Properties -> C/C++ -> Preprocessor Definitions
#pragma once

// ── Loading Screen ────────────────────────────────────────────────
#define DEBUG_LOG
// Keep the loading screen visible forever so you can inspect it.
// The game loop never starts. Close the window to exit.
 //#define DEV_FREEZE_LOADING_SCREEN

// Print detailed ICO parsing info to the console.
// #define DEV_ICON_VERBOSE

// ── General ───────────────────────────────────────────────────────

// Show a frame-rate counter in the top-right corner.
// (Also controllable via Config.prc show-frame-rate-meter)
// #define DEV_SHOW_FPS

// Skip the loading screen entirely and jump straight into gameplay.
// #define DEV_SKIP_LOADING_SCREEN

// Print game-loop timing stats every 5 seconds.
// #define DEV_TIMING_STATS

// ── Collision / Physics ───────────────────────────────────────────

// Draw collision geometry as wireframes.
// #define DEV_SHOW_COLLISION

// ── Feel free to add your own flags below ─────────────────────────