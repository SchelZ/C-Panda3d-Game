#include "hud.h"

void HUD::init(WindowFramework* window) {
    m_root = window->get_aspect_2d();

    // Score (top-left)
    m_score_text = new TextNode("score_text");
    m_score_text->set_text("Crystals: 0 / 0");
    m_score_text->set_align(TextNode::A_left);
    m_score_text->set_text_color(1.0f, 1.0f, 1.0f, 1.0f);
    m_score_text->set_shadow(0.05f, 0.05f);
    m_score_text->set_shadow_color(0, 0, 0, 0.8f);

    m_score_np = m_root.attach_new_node(m_score_text);
    m_score_np.set_scale(0.07f);
    m_score_np.set_pos(-1.3f, 0, 0.92f);

    // Timer (top-right)
    m_timer_text = new TextNode("timer_text");
    m_timer_text->set_text("Time: 0.0s");
    m_timer_text->set_align(TextNode::A_right);
    m_timer_text->set_text_color(1.0f, 1.0f, 0.8f, 1.0f);
    m_timer_text->set_shadow(0.05f, 0.05f);
    m_timer_text->set_shadow_color(0, 0, 0, 0.8f);

    m_timer_np = m_root.attach_new_node(m_timer_text);
    m_timer_np.set_scale(0.07f);
    m_timer_np.set_pos(1.3f, 0, 0.92f);

    // Center message (win/instructions)
    m_message_text = new TextNode("message_text");
    m_message_text->set_text("");
    m_message_text->set_align(TextNode::A_center);
    m_message_text->set_text_color(1.0f, 0.95f, 0.3f, 1.0f);
    m_message_text->set_shadow(0.05f, 0.05f);
    m_message_text->set_shadow_color(0, 0, 0, 0.9f);

    m_message_np = m_root.attach_new_node(m_message_text);
    m_message_np.set_scale(0.1f);
    m_message_np.set_pos(0, 0, 0);
    m_message_np.hide();
}

void HUD::update_score(int collected, int total) {
    m_score_text->set_text(std::format("Crystals: {} / {}", collected, total));
}

void HUD::update_timer(float elapsed) {
    m_timer_text->set_text(std::format("Time: {:.1f}s", elapsed));
}

void HUD::show_message(const std::string& msg) {
    m_message_text->set_text(msg);
    m_message_np.show();
}

void HUD::hide_message() {
    m_message_np.hide();
}
