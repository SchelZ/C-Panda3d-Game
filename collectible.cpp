// collectible.cpp — Spinning crystal collectible implementation
#include "collectible.h"
#include "filename.h"
#include "executionEnvironment.h"

LPoint3 Collectible::random_position() const {
    float range = GameConst::BOUNDARY - GameConst::SPAWN_MARGIN;
    float x = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f * range - range;
    float y = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f * range - range;
    return LPoint3(x, y, 0);
}

void Collectible::init(WindowFramework* window, NodePath parent, int index) {
    m_window       = window;
    m_index        = index;
    m_phase_offset = static_cast<float>(index) * 0.7f;
    m_base_pos     = random_position();
    m_active       = true;
 
    NodePath model = m_window->load_model(m_window->get_panda_framework()->get_models(), "box.egg.pz");
 
    if (!model.is_empty()) {
        model.reparent_to(parent);
        m_node = model;
    } 
 
    m_node.set_p(45.0f);
    m_node.set_r(45.0f);
 
    float hue = static_cast<float>(index % 5) / 5.0f;
    float r = 0.5f + 0.5f * std::sin(hue * 6.28f);
    float g = 0.5f + 0.5f * std::sin(hue * 6.28f + 2.09f);
    float b = 0.5f + 0.5f * std::sin(hue * 6.28f + 4.19f);
    m_node.set_color(r, g, b, 1.0f);
    m_node.set_pos(m_base_pos.get_x(), m_base_pos.get_y(), 1.5f);
}


void Collectible::update(float /*dt*/, float total_time) {
    if (!m_active) return;
    m_node.set_h(total_time * GameConst::CRYSTAL_SPIN_SPEED + m_index * 30.0f);
    float bob = std::sin(total_time * GameConst::CRYSTAL_BOB_SPEED + m_phase_offset) * GameConst::CRYSTAL_BOB_AMP;
    m_node.set_z(1.5f + bob);
}


void Collectible::collect() {
    m_active = false;
    m_node.hide();
}

void Collectible::respawn() {
    m_base_pos = random_position();
    m_node.set_pos(m_base_pos.get_x(), m_base_pos.get_y(), 1.5f);
    m_active = true;
    m_node.show();
}