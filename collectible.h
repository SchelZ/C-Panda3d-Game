// collectible.h — Spinning crystal collectible
#pragma once
#include "common.h"

class Collectible {
public:
    Collectible() = default;

    void init(WindowFramework* window, NodePath parent, int index);
    void update(float dt, float total_time);
    void collect();
    void respawn();

    bool        is_active() const { return m_active; }
    LPoint3     get_position() const { return m_base_pos; }
    NodePath    get_node() const { return m_node; }

private:
    WindowFramework* m_window = nullptr;

    NodePath    m_node;
    LPoint3     m_base_pos{0, 0, 0};
    bool        m_active = true;
    int         m_index = 0;
    float       m_phase_offset = 0.0f;

    LPoint3 random_position() const;
};
