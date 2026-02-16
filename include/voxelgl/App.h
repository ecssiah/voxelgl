#pragma once

#include "app/Camera.h"
#include "render/Renderer.h"



namespace voxelgl 
{

class App 
{

public:
    
    bool start();
    void run();
    void exit();

private:

    Camera m_camera;
    Renderer m_renderer;

    void update(const double dt);
    void render();

};

} // namespace voxelgl