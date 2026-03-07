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

    return app;
}

bool app_init(App* app)
{
    window_init(app->window);
    window_create_glfw_window(app->window);

    if (!gladLoadGLLoader((GLADloadproc)window_get_proc_address)) 
    {
        printf("Failed to initialize GLAD\n");

        return false;
    }

    input_init(app->input);
    renderer_init(app->renderer);
    
    world_init(app->world);

    return true;
}

void app_run(App* app) 
{
    while (!window_should_close(app->window)) 
    {
        input_begin_frame(app->input);
        window_poll_events(app->window);

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
    
    world_update(app->world, app->input, dt);
    camera_update(app->camera, app->input, dt);
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