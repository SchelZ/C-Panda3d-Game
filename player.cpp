// player.cpp — Player entity implementation
#include "player.h"
#include "filename.h"
#include "executionEnvironment.h"

void Player::init(WindowFramework* window, NodePath parent) {
    m_window    = window;
    m_node      = parent.attach_new_node("player");
    m_start_pos = LPoint3(0, 0, 0);
    m_node.set_pos(m_start_pos);
    m_heading   = 0.0f;

    create_model();
}

void Player::create_model() {
    NodePath model = m_window->load_model(m_window->get_panda_framework()->get_models(), "smiley.egg.pz");
 
    if (!model.is_empty()) {
        model.reparent_to(m_node);
        m_model = model;
    }
 
    if (!m_model.is_empty()) {
        m_model.set_scale(GameConst::PLAYER_SCALE * 3.0f);
        m_model.set_color(0.2f, 0.6f, 1.0f, 1.0f);
        m_model.set_pos(0, 0, 1.0f);
    }
}


void Player::update(float dt, const InputHandler& input) {
    // Rotation
    if (input.is_left()) {
        m_heading += GameConst::PLAYER_TURN_SPEED * dt;
    }
    if (input.is_right()) {
        m_heading -= GameConst::PLAYER_TURN_SPEED * dt;
    }
    m_node.set_h(m_heading);

    // Forward/backward movement
    float move = 0.0f;
    if (input.is_forward())  move += input.is_sprint() ? GameConst::PLAYER_SPEED : GameConst::PLAYER_SPEED * 0.6f;
    if (input.is_backward()) move -= GameConst::PLAYER_SPEED * 0.6f;

    if (std::abs(move) > 0.001f) {
        float rad = m_heading * GameConst::DEG_TO_RAD;
        float dx = -std::sin(rad) * move * dt;
        float dy = std::cos(rad) * move * dt;

        LPoint3 pos = m_node.get_pos();
        pos.set_x(pos.get_x() + dx);
        pos.set_y(pos.get_y() + dy);
        m_node.set_pos(pos);
    }

    clamp_to_bounds();
}

void Player::clamp_to_bounds() {
    LPoint3 pos = m_node.get_pos();
    float b = GameConst::BOUNDARY;

    pos.set_x(std::max(-b, std::min(b, pos.get_x())));
    pos.set_y(std::max(-b, std::min(b, pos.get_y())));

    m_node.set_pos(pos);
}

void Player::reset() {
    m_node.set_pos(m_start_pos);
    m_heading = 0.0f;
    m_node.set_h(0);
}