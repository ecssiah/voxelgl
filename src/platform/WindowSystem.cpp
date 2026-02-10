#include "platform/WindowSystem.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <sstream>
#include <iostream>

double WindowSystem::s_last_time_in_seconds { 0.0 };
GLFWwindow* WindowSystem::s_window { nullptr };

bool WindowSystem::init_library() 
{
    return glfwInit();
}

bool WindowSystem::create(int width, int height, const char* title) 
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    s_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!s_window) 
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();

        return false;
    }

    glfwMakeContextCurrent(s_window);

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

void WindowSystem::swap_buffers() 
{
    glfwSwapBuffers(s_window);
}

void WindowSystem::poll_events() 
{
    glfwPollEvents();

    if (glfwGetKey(s_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
    {
        glfwSetWindowShouldClose(s_window, GLFW_TRUE);
    }
}

void* WindowSystem::get_proc_address(const char* name) 
{
    return (void*)glfwGetProcAddress(name);
}

std::string WindowSystem::get_version_info()
{
    std::ostringstream ss;

    const char* gl_version { 
        reinterpret_cast<const char*>(glGetString(GL_VERSION)) 
    };

    const char* glsl_version { 
        reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)) 
    };

    const char* glfw_version { 
        reinterpret_cast<const char*>(glfwGetVersionString()) 
    };

    const char* vendor_version { 
        reinterpret_cast<const char*>(glGetString(GL_VENDOR))
    };

    const char* renderer_version { 
        reinterpret_cast<const char*>(glGetString(GL_RENDERER))
    };

    ss << "\n";
    ss << "OpenGL: " << "\n  " << gl_version << "\n";
    ss << "GLSL: "  << "\n  " << glsl_version << "\n";
    ss << "GLFW: "  << "\n  " << glfw_version << "\n";
    ss << "Vendor: "  << "\n  " << vendor_version << "\n";
    ss << "Renderer: "  << "\n  " << renderer_version << "\n";
    ss << "\n";

    return ss.str();
}

float WindowSystem::get_delta_seconds() 
{
    const double time_in_seconds { glfwGetTime() };

    const float dt { 
        static_cast<float>(time_in_seconds - s_last_time_in_seconds) 
    };

    s_last_time_in_seconds = time_in_seconds;
    
    return dt;
}