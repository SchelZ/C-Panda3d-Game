#include "game_app.h"

bool GameApp::init(int argc, char* argv[]) {
    m_framework.open_framework(argc, argv);
    m_framework.set_window_title(std::string{ GameConst::WINDOW_TITLE });

    WindowProperties wp;
    wp.set_size(GameConst::WINDOW_WIDTH, GameConst::WINDOW_HEIGHT);
    wp.set_title(std::string{ GameConst::WINDOW_TITLE });

    m_window = m_framework.open_window(wp, 0);
    if (!m_window) {
        std::cerr << "ERROR: Failed to open Panda3D window.\n";
        return false;
    }

    m_window->enable_keyboard();
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    m_input.init(m_window);     // Input

    // Loading screen
#ifdef DEV_SKIP_LOADING_SCREEN
    std::cout << "[DEV] DEV_SKIP_LOADING_SCREEN — skipping loading screen.\n";
    m_world.init(m_window);
    m_player.init(m_window, m_world.get_root());
    spawn_crystals();
    m_hud.init(m_window);
#else
    m_loading_screen.init(m_window);

    m_world.init(m_window);

    m_player.init(m_window, m_world.get_root());

    spawn_crystals();

    m_hud.init(m_window);

#endif

    setup_camera();
    m_hud.update_score(0, GameConst::NUM_CRYSTALS);
    m_hud.show_message("WASD/Arrows to move\nCollect all crystals!");

    m_loading_screen.dismiss();     // exit loading screen

    // ── DEV: FPS meter ──────────────────────
#ifdef DEV_SHOW_FPS
    std::cout << "[DEV] DEV_SHOW_FPS — enabling frame rate meter.\n";
    m_framework.get_graphics_engine()->set_frame_rate_meter(true);
#endif

    // ── Game update task ────────────────────
    m_task_mgr = AsyncTaskManager::get_global_ptr();
    PT(GenericAsyncTask) task = new GenericAsyncTask(
        "game_update", &GameApp::game_update_task, this);
    task->set_sort(0);
    m_task_mgr->add(task);

    return true;
}

void GameApp::setup_camera() {
    NodePath cam = m_window->get_camera_group();
    m_cam_pos = LPoint3(0, -GameConst::CAM_DISTANCE, GameConst::CAM_HEIGHT);
    cam.set_pos(m_cam_pos);
    cam.look_at(m_player.get_node(), LPoint3(0, 0, GameConst::CAM_LOOK_OFFSET));
}

void GameApp::spawn_crystals() {
    m_crystals.resize(GameConst::NUM_CRYSTALS);
    for (int i = 0; i < GameConst::NUM_CRYSTALS; ++i) {
        m_crystals[i].init(m_window, m_world.get_root(), i);
    }
}

void GameApp::check_collections() {
    LPoint3 player_pos = m_player.get_position();
 
    for (auto& crystal : m_crystals) {
        if (!crystal.is_active()) continue;
 
        LPoint3 crystal_pos = crystal.get_position();
        float dx = player_pos.get_x() - crystal_pos.get_x();
        float dy = player_pos.get_y() - crystal_pos.get_y();
        float dist_sq = dx * dx + dy * dy;
 
        if (dist_sq < GameConst::COLLECT_DIST_SQ) {
            crystal.collect();
            m_score++;
            m_hud.update_score(m_score, GameConst::NUM_CRYSTALS);
 
            if (m_score >= GameConst::NUM_CRYSTALS) {
                m_state = GameState::Won;
                m_hud.show_message(std::format(
                    "YOU WIN!\nTime: {:.1f} seconds\nPress R to play again",
                    m_elapsed_time));
            }
        }
    }
}

void GameApp::update_camera(float dt) {
    NodePath cam = m_window->get_camera_group();
    LPoint3 player_pos = m_player.get_position();

    float heading_rad = m_player.get_heading() * GameConst::DEG_TO_RAD;
    float target_x = player_pos.get_x() + std::sin(heading_rad) * GameConst::CAM_DISTANCE;
    float target_y = player_pos.get_y() - std::cos(heading_rad) * GameConst::CAM_DISTANCE;

    LPoint3 target(target_x, target_y, GameConst::CAM_HEIGHT);
    float t = std::min(1.0f, GameConst::CAM_SMOOTH * dt);
    m_cam_pos = m_cam_pos + (target - m_cam_pos) * t;

    cam.set_pos(m_cam_pos);
    cam.look_at(m_player.get_node(), LPoint3(0, 0, GameConst::CAM_LOOK_OFFSET));
}

void GameApp::restart_game() {
    m_score = 0;
    m_elapsed_time = 0.0f;
    m_state = GameState::Playing;

    m_player.reset();
    for (auto& crystal : m_crystals) { crystal.respawn(); }

    m_hud.update_score(0, GameConst::NUM_CRYSTALS);
    m_hud.hide_message();
    setup_camera();
}

AsyncTask::DoneStatus GameApp::game_update_task(GenericAsyncTask* /*task*/, void* data) {
    auto* app = static_cast<GameApp*>(data);
    ClockObject* clock = ClockObject::get_global_clock();
    float dt = static_cast<float>(clock->get_dt());
    app->m_total_time += dt;
    dt = std::min(dt, 0.05f);

    if (app->m_input.is_quit()) {
        app->m_framework.set_exit_flag();
        return AsyncTask::DS_done;
    }

    if (app->m_input.is_restart()) {
        app->restart_game();
        return AsyncTask::DS_cont;
    }

    if (app->m_state == GameState::Playing && app->m_elapsed_time > 0.5f) {
        app->m_hud.hide_message();
    }

    if (app->m_state == GameState::Playing) {
        app->m_elapsed_time += dt;
        app->m_player.update(dt, app->m_input);
        app->check_collections();
        app->m_hud.update_timer(app->m_elapsed_time);
    }

    for (auto& crystal : app->m_crystals) {
        crystal.update(dt, app->m_total_time);
    }

    app->update_camera(dt);

#ifdef DEV_TIMING_STATS
    // Print frame timing every 5 seconds
    static float s_stats_timer = 0.0f;
    s_stats_timer += dt;
    if (s_stats_timer >= 5.0f) {
        s_stats_timer = 0.0f;
        std::cout << std::format(
            "[DEV] t={:.1f}s  dt={:.2f}ms  fps={:.1f}\n",
            app->m_total_time,
            dt * 1000.0f,
            dt > 0.0f ? 1.0f / dt : 0.0f);
    }
#endif

    return AsyncTask::DS_cont;
}

void GameApp::run() {
    m_framework.main_loop();
}

void GameApp::shutdown() {
    m_framework.close_framework();
}