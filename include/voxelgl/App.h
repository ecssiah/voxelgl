#pragma once

#include "app/Camera.h"
#include "app/world/World.h"
#include "render/Renderer.h"

namespace voxelgl 
{

class App 
{

public:
    
    bool init();
    void run();
    void exit();

private:

    Camera m_camera;
    Renderer m_renderer;

    World m_world;

    void update(double dt);
    void render();

};

} // namespace voxelgl