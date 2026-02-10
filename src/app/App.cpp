#include "voxelgl/App.h"

#include "../platform/WindowSystem.h"

#include <iostream>

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

    m_renderer.start();

    return true;
}

void App::run() 
{
    while (!WindowSystem::should_close()) 
    {
        const float dt { WindowSystem::get_delta_seconds() };

        update(dt);
        render(dt);

        WindowSystem::swap_buffers();
        WindowSystem::poll_events();
    }
}

void App::exit()
{
    WindowSystem::destroy();
}

void App::update(const float dt) 
{

}

void App::render(const float dt) 
{
    m_renderer.render(dt);
}

} // namespace voxelgl