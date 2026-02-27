#pragma once

#include "app/camera.h"
#include "app/world/world.h"
#include "render/renderer.h"

namespace voxelgl 
{

struct App 
{
    Camera m_camera;
    Renderer m_renderer;

    World m_world;
    
    bool init();
    void run();
    void exit();

    void update(double dt);
    void render();
};

} // namespace voxelgl