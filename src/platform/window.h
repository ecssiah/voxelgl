#pragma once

#include "core/types.h"

struct GLFWwindow;

static const u32 WINDOW_WIDTH = 1024;
static const u32 WINDOW_HEIGHT = 768;

struct Window
{
    GLFWwindow* glfw_window;

    f64 last_time;
};

Window* window_create();
void window_init(Window* window);
void window_destroy(Window* window);

void window_create_glfw_window(Window* window);
bool window_should_close(Window* window);
void window_request_close(Window* window);
void window_poll_events();
void window_swap_buffers(Window* window);
f64 window_calculate_delta_time(Window* window);

f32 window_get_aspect_ratio();
void window_set_cursor_enabled(Window* window, bool enabled);
void* window_get_proc_address(const char* name);
void window_print_info();

static void glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
static void glfw_mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods);
static void glfw_cursor_position_callback(GLFWwindow* glfw_window, f64 xpos, f64 ypos);