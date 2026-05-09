#pragma once
#include "common.h"
#include "input_handler.h"
#include "player.h"
#include "collectible.h"
#include "world.h"
#include "hud.h"
#include "loading_screen.h"

enum class GameState {
    Playing,
    Won
};

class GameApp {
public:
    GameApp() = default;

    bool init(int argc, char* argv[]);
    void run();
    void shutdown();

private:
    // Core Panda3D
    PandaFramework       m_framework;
    WindowFramework*     m_window = nullptr;
    PT(AsyncTaskManager) m_task_mgr;

    // Game systems
    InputHandler        m_input;
    Player              m_player;
    World               m_world;
    HUD                 m_hud;
    LoadingScreen       m_loading_screen;
    std::vector<Collectible> m_crystals;

    // Game state
    GameState           m_state = GameState::Playing;
    int                 m_score = 0;
    float               m_elapsed_time = 0.0f;
    float               m_total_time = 0.0f;

    // Camera
    LPoint3             m_cam_pos{ 0, 0, 0 };

    void setup_camera();
    void spawn_crystals();
    void check_collections();
    void update_camera(float dt);
    void restart_game();

    // Panda3D task callback
    static AsyncTask::DoneStatus game_update_task(GenericAsyncTask* task, void* data);
};