#include "voxelgl/App.h"

#include "../platform/InputSystem.h"
#include "../platform/WindowSystem.h"
#include "render/Renderer.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>



namespace voxelgl
{

bool App::start()
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

    WindowSystem::set_cursor_enabled(false);

    if (!gladLoadGLLoader((GLADloadproc)WindowSystem::get_proc_address)) 
    {
        std::cerr << "Failed to initialize GLAD\n";

        return false;
    }

    m_camera.set_position(0.0f, 0.0f, 5.0f);
    m_camera.set_yaw(-90.0f);
    m_camera.set_pitch(0.0f);

    m_camera.set_perspective(
        glm_rad(60.0f),
        WindowSystem::get_aspect_ratio(),
        0.1f,
        1000.0f
    );

    m_renderer.start();

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

void App::update(const double dt) 
{
    if (InputSystem::is_key_pressed(GLFW_KEY_ESCAPE) == GL_TRUE)
    {
        WindowSystem::request_close();
    }

    m_camera.update(dt);
}

void App::render()
{
    mat4 view_matrix, projection_matrix;
    m_camera.get_view_matrix(view_matrix);
    m_camera.get_projection_matrix(projection_matrix);

    m_renderer.render(view_matrix, projection_matrix);
}

}