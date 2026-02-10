#include "voxelgl/App.h"

#include "../platform/WindowSystem.h"

#include <glad/glad.h>
#include <iostream>

namespace voxelgl {

static constexpr int WINDOW_WIDTH  = 800;
static constexpr int WINDOW_HEIGHT = 600;

bool App::initialize() {
    if (!WindowSystem::init_library()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    if (!WindowSystem::create(WINDOW_WIDTH, WINDOW_HEIGHT, "VoxelGL")) {
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)WindowSystem::get_proc_address)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    std::cout << "OpenGL version: "
              << glGetString(GL_VERSION) << "\n";

    return true;
}

void App::run() {
    while (!WindowSystem::should_close()) {
        update(0.0f);   // dt later
        render();

        WindowSystem::swap_buffers();
        WindowSystem::poll_events();
    }
}

void App::shutdown() {
    WindowSystem::destroy();
}

void App::update(float /*dt*/) {
    // input, simulation, camera later
}

void App::render() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace voxelgl