#pragma once

#include <string>

struct GLFWwindow;

class WindowSystem 
{

public:

    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

    static bool init_library();
    static bool create(const char* title);
    static void destroy();

    static bool should_close();
    static void request_close();
    
    static void swap_buffers();
    static void poll_events();

    static void* get_proc_address(const char* name);

    static std::string get_version_info();

    static float get_aspect_ratio();
    static float get_delta_seconds();

    static void set_cursor_enabled(bool enabled);

private:

    static GLFWwindow* s_window;

    static double s_last_time_in_seconds;

};