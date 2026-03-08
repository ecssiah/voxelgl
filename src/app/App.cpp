#include "app/app.h"

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "platform/input.h"
#include "app/camera.h"
#include "platform/window.h"
#include "render/renderer.h"

App* app_create()
{
    App* app = (App*)malloc(sizeof (App));

    app->window = window_create();
    app->input = input_create();
    app->camera = camera_create();
    app->world = world_create();
    app->renderer = renderer_create();

    return app;
}

bool app_init(App* app)
{
    window_init(app->window);
    input_init(app->input);
    camera_init(app->camera);
    world_init(app->world);
    renderer_init(app->renderer);

    glfwSetWindowUserPointer(app->window->glfw_window, app->input);

    return true;
}

void app_run(App* app) 
{
    while (!window_should_close(app->window)) 
    {
        input_begin_frame(app->input);

        window_poll_events();

        f64 dt = window_calculate_delta_time(app->window);

        app_update(app, dt);
        app_render(app);

        window_swap_buffers(app->window);
    }
}

void app_update(App* app, f64 dt)
{
    if (input_is_key_pressed(app->input, GLFW_KEY_ESCAPE) == GL_TRUE)
    {
        window_request_close(app->window);
    }
    
    camera_update(app->camera, app->input, dt);
    world_update(app->world, app->input, dt);
    
    renderer_update(app->renderer, app->world);
}

void app_render(App* app)
{
    mat4 view_matrix, projection_matrix;
    camera_get_view_matrix(app->camera, view_matrix);
    camera_get_projection_matrix(app->camera, projection_matrix);

    renderer_render(app->renderer, view_matrix, projection_matrix);
}

void app_destroy(App* app)
{
    free(app);
}