// world.cpp — Scene setup implementation
#include "world.h"
#include "filename.h"
#include "executionEnvironment.h"
#include <array>

void World::init(WindowFramework* window) {
    m_window = window;
    m_root   = window->get_render().attach_new_node("world_root");
    create_ground();
    create_lighting();
    create_fog(window);
    create_boundary_markers();
    m_root.flatten_strong();
}

void World::create_ground() {
    CardMaker cm("ground");
    float half = GameConst::GROUND_SIZE;
    cm.set_frame(-half, half, -half, half);
    NodePath ground = m_root.attach_new_node(cm.generate());
    ground.set_p(-90);
    ground.set_z(-0.01f);
    ground.set_color(0.25f, 0.55f, 0.25f, 1.0f);

    CardMaker cm2("ground_inner");
    float inner = GameConst::BOUNDARY;
    cm2.set_frame(-inner, inner, -inner, inner);
    NodePath inner_ground = m_root.attach_new_node(cm2.generate());
    inner_ground.set_p(-90);
    inner_ground.set_z(0.0f);
    inner_ground.set_color(0.3f, 0.6f, 0.3f, 1.0f);
}

void World::create_lighting() {
    PT(AmbientLight) ambient = new AmbientLight("ambient");
    ambient->set_color(LColor(0.35f, 0.35f, 0.4f, 1.0f));
    m_root.set_light(m_root.attach_new_node(ambient));

    PT(DirectionalLight) sun = new DirectionalLight("sun");
    sun->set_color(LColor(0.95f, 0.9f, 0.8f, 1.0f));
    NodePath sun_np = m_root.attach_new_node(sun);
    sun_np.set_hpr(-30, -60, 0);
    m_root.set_light(sun_np);

    PT(DirectionalLight) fill = new DirectionalLight("fill");
    fill->set_color(LColor(0.3f, 0.35f, 0.5f, 1.0f));
    NodePath fill_np = m_root.attach_new_node(fill);
    fill_np.set_hpr(150, -30, 0);
    m_root.set_light(fill_np);
}

void World::create_fog(WindowFramework* window) {
    PT(Fog) fog = new Fog("distance_fog");
    fog->set_color(0.6f, 0.75f, 0.85f);
    fog->set_exp_density(0.008f);
    window->get_render().set_fog(fog);
    window->get_graphics_window()->set_clear_color(
        LColor(0.6f, 0.75f, 0.85f, 1.0f));
}

void World::create_boundary_markers() {
    NodePath post_template = m_window->load_model(m_window->get_panda_framework()->get_models(), "box.egg.pz");

    constexpr float b = GameConst::BOUNDARY;
    constexpr std::array<std::pair<float, float>, 8> positions {{
        {-b, -b}, {-b, 0.0f}, {-b,  b},
        { b, -b}, { b, 0.0f}, { b,  b},
        { 0.0f, -b}, { 0.0f, b}
    }};

    int i = 0;
    for (const auto& [x, y] : positions) {
        NodePath post;
        if (!post_template.is_empty()) {
            post = post_template.copy_to(m_root);
        }
        post.set_pos(x, y, 1.5f);
        post.set_scale(0.5f, 0.5f, 3.0f);
        post.set_color(0.6f, 0.4f, 0.2f, 1.0f);
        ++i;
    }
}