#pragma once
#include "common.h"
#include "cardMaker.h"
#include "texture.h"
#include "pnmImage.h"
#include "graphicsOutput.h"
#include "graphicsWindow.h"
#include "graphicsEngine.h"
#include "icon_data.h"
#include "colorAttrib.h"
#include "transparencyAttrib.h"

class LoadingScreen {
public:
    LoadingScreen() = default;
 
    void init(WindowFramework* window);
    void dismiss();
    void render_frame();    
private:
    WindowFramework*  m_window  = nullptr;
 
    NodePath  m_splash_root;
    NodePath  m_icon_node;
 
    bool      m_active        = false;
 
    // Splash timing
    static constexpr float FADE_IN_TIME  = 0.8f;
    static constexpr float HOLD_TIME     = 1.2f;
    static constexpr float FADE_OUT_TIME = 0.6f;

    void set_alpha(float a);
    void create_splash();
    void run_fade_in();   // called by init()   — fade in + hold
    void run_fade_out();  // called by dismiss() — fade out
};