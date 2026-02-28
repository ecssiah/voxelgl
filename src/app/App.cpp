#include "voxelgl/app.h"
#include "app/world/cell.h"
#include "platform/input_system.h"
#include "platform/window_system.h"
#include "render/renderer.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

namespace voxelgl
{

bool App::init()
{
    if (!WindowSystem::init()) 
    {
        std::cerr << "Failed to initialize\n";

        return false;
    }

    if (!WindowSystem::create("VoxelGL")) 
    {
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)WindowSystem::get_proc_address)) 
    {
        std::cerr << "Failed to initialize GLAD\n";

        return false;
    }

    if (!m_camera.init())
    {
        return false;
    }

    if (!m_renderer.init())
    {
        return false;
    }

    if (!m_world.init())
    {
        return false;
    }
    
    m_world.set_block_kind(ivec3{0, 1, 0}, BLOCK_KIND_HORSE);
    m_world.set_block_kind(ivec3{0, -1, 0}, BLOCK_KIND_LION);
    m_world.set_block_kind(ivec3{1, 0, 0}, BLOCK_KIND_WOLF);
    m_world.set_block_kind(ivec3{-1, 0, 0}, BLOCK_KIND_EAGLE);

    m_world.set_block_kind_wireframe(
        ivec3{-4, -4, -4}, 
        ivec3{4, 4, 4}, 
        BLOCK_KIND_LION
    );

    return true;
}

void App::run() 
{
    while (!WindowSystem::should_close()) 
    {
        InputSystem::begin_frame();
        WindowSystem::poll_events();

        const double dt = WindowSystem::get_dt();

        update(dt);
        render();

        WindowSystem::swap_buffers();
    }
}

void App::exit()
{
    WindowSystem::destroy();
}

void App::update(double dt) 
{
    if (InputSystem::is_key_pressed(GLFW_KEY_ESCAPE) == GL_TRUE)
    {
        WindowSystem::request_close();
    }

    m_camera.update(dt);
    m_renderer.update(&m_world);
}

void App::render()
{
    mat4 view_matrix, projection_matrix;
    m_camera.get_view_matrix(view_matrix);
    m_camera.get_projection_matrix(projection_matrix);

    m_renderer.render(view_matrix, projection_matrix);
}

}