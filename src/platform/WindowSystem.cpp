#include "platform/WindowSystem.h"

#include <GLFW/glfw3.h>
#include <iostream>

GLFWwindow* WindowSystem::s_window = nullptr;

bool WindowSystem::init_library() {
    return glfwInit();
}

bool WindowSystem::create(int width, int height, const char* title) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    s_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!s_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(s_window);

    return true;
}

void WindowSystem::destroy() {
    if (s_window) {
        glfwDestroyWindow(s_window);
        s_window = nullptr;
    }
    glfwTerminate();
}

bool WindowSystem::should_close() {
    return glfwWindowShouldClose(s_window);
}

void WindowSystem::swap_buffers() {
    glfwSwapBuffers(s_window);
}

void WindowSystem::poll_events() {
    glfwPollEvents();
}

void* WindowSystem::get_proc_address(const char* name) {
    return (void*)glfwGetProcAddress(name);
}