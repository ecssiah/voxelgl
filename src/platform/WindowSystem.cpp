#include "platform/WindowSystem.h"
#include "platform/InputSystem.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <sstream>
#include <iostream>

double WindowSystem::s_last_time = 0.0;

GLFWwindow* WindowSystem::s_window = nullptr;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    InputSystem::on_key(key, scancode, action, mods);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    InputSystem::on_mouse_button(button, action, mods);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    InputSystem::on_cursor_position(xpos, ypos);
}

bool WindowSystem::init() 
{
    return glfwInit();
}

bool WindowSystem::create(const char* title) 
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    s_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, nullptr, nullptr);

    if (!s_window) 
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();

        return false;
    }

    glfwMakeContextCurrent(s_window);

    glfwSetKeyCallback(s_window, key_callback);
    glfwSetMouseButtonCallback(s_window, mouse_button_callback);
    glfwSetCursorPosCallback(s_window, cursor_position_callback);

    return true;
}

void WindowSystem::destroy() 
{
    if (s_window) 
    {
        glfwDestroyWindow(s_window);
        s_window = nullptr;
    }
    
    glfwTerminate();
}

bool WindowSystem::should_close() 
{
    return glfwWindowShouldClose(s_window);
}

void WindowSystem::request_close()
{
    glfwSetWindowShouldClose(s_window, GLFW_TRUE);
}

void WindowSystem::swap_buffers() 
{
    glfwSwapBuffers(s_window);
}

void WindowSystem::poll_events() 
{
    glfwPollEvents();
}

void* WindowSystem::get_proc_address(const char* name) 
{
    return (void*)glfwGetProcAddress(name);
}

std::string WindowSystem::get_version_info()
{
    std::ostringstream ss;

    const char* gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const char* glsl_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    const char* glfw_version = reinterpret_cast<const char*>(glfwGetVersionString());
    const char* vendor_version = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* renderer_version = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

    ss << "\n";
    ss << "OpenGL: " << "\n  " << gl_version << "\n";
    ss << "GLSL: "  << "\n  " << glsl_version << "\n";
    ss << "GLFW: "  << "\n  " << glfw_version << "\n";
    ss << "Vendor: "  << "\n  " << vendor_version << "\n";
    ss << "Renderer: "  << "\n  " << renderer_version << "\n";
    ss << "\n";

    return ss.str();
}

float WindowSystem::get_aspect_ratio()
{
    return WINDOW_WIDTH / static_cast<float>(WINDOW_HEIGHT);
}

double WindowSystem::get_dt() 
{
    const double current_time = glfwGetTime();

    const double dt = current_time - s_last_time;

    s_last_time = current_time;
    
    return dt;
}

void WindowSystem::set_cursor_enabled(bool enabled)
{
    glfwSetInputMode(
        s_window,
        GLFW_CURSOR,
        enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED
    );
}