#include "loading_screen.h"
#include "pnmImage.h"


void LoadingScreen::init(WindowFramework* window) {
    m_window = window;
    m_window->get_graphics_window()->set_clear_color(LColor(0, 0, 0, 1));
    create_splash();
}

void LoadingScreen::dismiss() {
    if (!m_active) return;

#ifndef DEV_FREEZE_LOADING_SCREEN
    run_fade_out();
#else
    std::cout << "[DEV] Splash animation complete - frozen on logo.\n"
              << "      Close the window to exit.\n";
    while (m_window && !m_window->get_graphics_window()->is_closed())
        render_frame();
#endif

    m_splash_root.remove_node();
    m_active = false;
    m_window->get_graphics_window()->set_clear_color(LColor(0.6f, 0.75f, 0.85f, 1.0f));
}

void LoadingScreen::render_frame() {
    if (m_window) GraphicsEngine::get_global_ptr()->render_frame();
}

void LoadingScreen::set_alpha(float a) {
    m_splash_root.set_color_scale(1, 1, 1, a);
}

void LoadingScreen::create_splash() {
    // Root node attached to aspect_2d for independent resolution
    m_splash_root = m_window->get_aspect_2d().attach_new_node("splash_root");
    m_splash_root.set_bin("fixed", 100);
    m_splash_root.set_depth_test(false);
    m_splash_root.set_depth_write(false);
 
    // Full-screen black background
    CardMaker bg("splash_bg");
    bg.set_frame(-2.0f, 2.0f, -2.0f, 2.0f);
    NodePath bg_np = m_splash_root.attach_new_node(bg.generate());
    bg_np.set_color(0, 0, 0, 1);
    bg_np.set_pos(0, 1, 0);
 
    // Icon card centred on screen
    CardMaker m_icon_card("icon");
    m_icon_card.set_frame(-0.3f, 0.3f, -0.3f, 0.3f);
    m_icon_card.set_has_uvs(true);

    // Icon node
    m_icon_node = m_splash_root.attach_new_node(m_icon_card.generate());
    m_icon_node.set_pos(0, 0, 0);

    // Icon texture we attach to Icon node
    PT(Texture) icon_texture = parse_ico_to_texture();
    if (icon_texture) {
        m_icon_node.set_texture(icon_texture);
        m_icon_node.set_transparency(TransparencyAttrib::M_alpha);
        m_icon_node.set_color_off();
    }
 
    // Start as invisible
    m_splash_root.set_transparency(TransparencyAttrib::M_alpha);
    set_alpha(0);
    m_active = true;

    run_fade_in();

#ifdef DEV_FREEZE_LOADING_SCREEN
    std::cout << "[DEV] DEV_FREEZE_LOADING_SCREEN active - frozen on splash.\n";
#endif
}

// Fade in + hold — called immediately after create_splash().
void LoadingScreen::run_fade_in() {
    constexpr float dt      = 1.0f / 60.0f;
    constexpr int   fade_in = static_cast<int>(FADE_IN_TIME / dt);
    constexpr int   hold    = static_cast<int>(HOLD_TIME    / dt);

    for (int i = 0; i <= fade_in; ++i) {
        float a = static_cast<float>(i) / fade_in;
        a = a * a * (3.0f - 2.0f * a);  // smoothstep
        set_alpha(a);
        render_frame();
    }

    set_alpha(1.0f);
    for (int i = 0; i < hold; ++i) render_frame();
}

// Fade out — called by dismiss(), after all assets are loaded.
void LoadingScreen::run_fade_out() {
    constexpr float dt       = 1.0f / 60.0f;
    constexpr int   fade_out = static_cast<int>(FADE_OUT_TIME / dt);

    for (int i = 0; i <= fade_out; ++i) {
        float a = 1.0f - static_cast<float>(i) / fade_out;
        a = a * a * (3.0f - 2.0f * a);
        set_alpha(a);
        render_frame();
    }
}