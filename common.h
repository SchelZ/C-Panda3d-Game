#pragma once

#include "dev_flags.h"

// ── Panda3D Core
#include "pandaFramework.h"
#include "pandaSystem.h"
#include "windowFramework.h"
#include "genericAsyncTask.h"
#include "asyncTaskManager.h"
#include "nodePath.h"
#include "cardMaker.h"
#include "textNode.h"
#include "ambientLight.h"
#include "directionalLight.h"
#include "pointLight.h"
#include "camera.h"
#include "clockObject.h"
#include "mouseWatcher.h"
#include "auto_bind.h"
#include "animControlCollection.h"
#include "collisionTraverser.h"
#include "collisionNode.h"
#include "collisionSphere.h"
#include "collisionHandlerPusher.h"
#include "collisionHandlerQueue.h"
#include "boundingSphere.h"
#include "loader.h"
#include "materialAttrib.h"
#include "material.h"
#include "fog.h"
#include "luse.h"

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <numbers>
#include <span>
#include <format>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <algorithm>
#include <iostream>
#include <functional>
#include <concepts>

// Game Constants
namespace GameConst {
    // Window
    constexpr int              WINDOW_WIDTH = 1280;
    constexpr int              WINDOW_HEIGHT = 720;
    constexpr std::string_view WINDOW_TITLE = "Game Test";

    // Player
    constexpr float PLAYER_SPEED = 12.0f;
    constexpr float PLAYER_TURN_SPEED = 150.0f;
    constexpr float PLAYER_SCALE = 0.3f;
    constexpr float PLAYER_RADIUS = 1.0f;

    // Camera
    constexpr float CAM_DISTANCE = 25.0f;
    constexpr float CAM_HEIGHT = 18.0f;
    constexpr float CAM_LOOK_OFFSET = 2.0f;
    constexpr float CAM_SMOOTH = 5.0f;

    // World
    constexpr float GROUND_SIZE = 80.0f;
    constexpr float BOUNDARY = 38.0f;

    // Collectibles
    constexpr int   NUM_CRYSTALS = 15;
    constexpr float CRYSTAL_SCALE = 0.6f;
    constexpr float CRYSTAL_RADIUS = 1.5f;
    constexpr float CRYSTAL_BOB_SPEED = 2.0f;
    constexpr float CRYSTAL_BOB_AMP = 0.4f;
    constexpr float CRYSTAL_SPIN_SPEED = 90.0f;
    constexpr float COLLECT_DISTANCE = 3.0f;
    constexpr float SPAWN_MARGIN = 5.0f;

    constexpr float COLLECT_DIST_SQ = COLLECT_DISTANCE * COLLECT_DISTANCE;
    constexpr float DEG_TO_RAD = std::numbers::pi_v<float> / 180.0f;
}