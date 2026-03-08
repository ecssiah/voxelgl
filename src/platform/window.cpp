#include "platform/window.h"
#include "platform/input.h"

#include <cstdio>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Window* window_create()
{
    Window* window = (Window*)malloc(sizeof (Window));

    return window;
}

void window_init(Window* window) 
{
    window->glfw_window = NULL;
    window->last_time = 0.0;

    glfwInit();

    window_create_glfw_window(window);

    window_print_info();
}

void window_create_glfw_window(Window* window) 
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    const char* window_title = "Voxel GL";

    GLFWwindow* glfw_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, window_title, NULL, NULL);

    if (glfw_window) 
    {
        glfwMakeContextCurrent(glfw_window);

        glfwSetKeyCallback(glfw_window, glfw_key_callback);
        glfwSetMouseButtonCallback(glfw_window, glfw_mouse_button_callback);
        glfwSetCursorPosCallback(glfw_window, glfw_cursor_position_callback);

        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        window->glfw_window = glfw_window;

        window_set_cursor_enabled(window, false);
    }
    else
    {
        printf("Failed to create GLFW window\n");

        glfwTerminate();
    }
}

void window_destroy(Window* window) 
{
    if (window->glfw_window) 
    {
        glfwDestroyWindow(window->glfw_window);
        window->glfw_window = NULL;
    }
    
    glfwTerminate();

    free(window);
}

bool window_should_close(Window* window) 
{
    return glfwWindowShouldClose(window->glfw_window);
}

void window_request_close(Window* window)
{
    glfwSetWindowShouldClose(window->glfw_window, GLFW_TRUE);
}

void window_swap_buffers(Window* window) 
{
    glfwSwapBuffers(window->glfw_window);
}

void window_poll_events() 
{
    glfwPollEvents();
}

void* window_get_proc_address(const char* name) 
{
    return (void*)glfwGetProcAddress(name);
}

void window_print_info()
{
    const char* gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const char* glsl_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    const char* glfw_version = reinterpret_cast<const char*>(glfwGetVersionString());
    const char* vendor_version = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* renderer_version = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

    printf(
        "\nOpenGL: %s\nGLSL: %s\nGLFW: %s\nVendor: %s, Renderer: %s\n\n", 
        gl_version,
        glsl_version,
        glfw_version,
        vendor_version,
        renderer_version
    );
}

f32 window_get_aspect_ratio()
{
    return (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT;
}

f64 window_calculate_delta_time(Window* window) 
{
    f64 current_time = glfwGetTime();
    f64 dt = current_time - window->last_time;

    window->last_time = current_time;
    
    return dt;
}

void window_set_cursor_enabled(Window* window, bool enabled)
{
    glfwSetInputMode(
        window->glfw_window,
        GLFW_CURSOR,
        enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED
    );
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input* input = (Input*)glfwGetWindowUserPointer(window);

    input_handle_key(input, key, scancode, action, mods);
}

static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Input* input = (Input*)glfwGetWindowUserPointer(window);

    input_handle_mouse_button(input, button, action, mods);
}

static void glfw_cursor_position_callback(GLFWwindow* window, f64 xpos, f64 ypos)
{
    Input* input = (Input*)glfwGetWindowUserPointer(window);

    input_handle_cursor_position(input, xpos, ypos);
}
