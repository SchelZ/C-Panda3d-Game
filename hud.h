#pragma once
#include "common.h"

class HUD {
public:
    HUD() = default;

    void init(WindowFramework* window);
    void update_score(int collected, int total);
    void update_timer(float elapsed);
    void show_message(const std::string& msg);
    void hide_message();

private:
    NodePath m_root;
    PT(TextNode) m_score_text;
    PT(TextNode) m_timer_text;
    PT(TextNode) m_message_text;
    NodePath m_score_np;
    NodePath m_timer_np;
    NodePath m_message_np;
};
