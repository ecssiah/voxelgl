#pragma once

#include "core/types.h"

struct Input;
struct Window;
struct Renderer;
struct Camera;
struct World;

struct App
{
    Input* input;
    Window* window;
    Renderer* renderer;
    Camera* camera;
    World* world;
};

App* app_create();
bool app_init(App* app);
void app_run(App* app);
void app_destroy(App* app);

void app_update(App* app, f64 dt);
void app_render(App* app);

void a_setup_demo_world(App* app);