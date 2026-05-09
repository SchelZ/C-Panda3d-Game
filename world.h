// world.h — Scene setup: ground, lighting, fog, boundaries
#pragma once
#include "common.h"

class World {
public:
    World() = default;

    void init(WindowFramework* window);
    NodePath get_root() const { return m_root; }

private:
    NodePath m_root;
    WindowFramework* m_window = nullptr;

    void create_ground();
    void create_lighting();
    void create_fog(WindowFramework* window);
    void create_boundary_markers();
};
