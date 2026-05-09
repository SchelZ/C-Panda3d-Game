// player.h — Player entity with movement and collision
#pragma once
#include "common.h"
#include "input_handler.h"

class Player {
public:
    Player() = default;

    void init(WindowFramework* window, NodePath parent);
    void update(float dt, const InputHandler& input);
    void reset();

    NodePath        get_node() const { return m_node; }
    LPoint3         get_position() const { return m_node.get_pos(); }
    float           get_heading() const { return m_heading; }

private:
    WindowFramework* m_window = nullptr;
    NodePath        m_node;
    NodePath        m_model;
    float           m_heading   = 0.0f;
    LPoint3         m_start_pos{0, 0, 0};

    void clamp_to_bounds();
    void create_model();
};