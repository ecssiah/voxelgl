#include "voxelgl/App.h"

#include "../platform/InputSystem.h"
#include "../platform/WindowSystem.h"
#include "render/Renderer.h"

#include <iostream>
#include <cglm/cglm.h>

namespace voxelgl 
{

static constexpr int WINDOW_WIDTH  = 800;
static constexpr int WINDOW_HEIGHT = 600;

bool App::start() 
{
    if (!WindowSystem::init_library()) 
    {
        std::cerr << "Failed to initialize GLFW\n";

        return false;
    }

    if (!WindowSystem::create(WINDOW_WIDTH, WINDOW_HEIGHT, "VoxelGL")) 
    {
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)WindowSystem::get_proc_address)) 
    {
        std::cerr << "Failed to initialize GLAD\n";

        return false;
    }

    std::cout << WindowSystem::get_version_info();

    m_camera.set_perspective(
        glm_rad(60.0f),
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
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

        update(WindowSystem::get_delta_seconds());
        render();

        WindowSystem::swap_buffers();
    }
}

void App::exit()
{
    WindowSystem::destroy();
}

void App::update(const float dt) 
{
    if (InputSystem::is_key_pressed(GLFW_KEY_ESCAPE) == GL_TRUE)
    {
        WindowSystem::request_close();
    }
}

void App::render() 
{
    m_renderer.render(m_camera.get_view_matrix(), m_camera.get_projection_matrix());
}

} // namespace voxelgl