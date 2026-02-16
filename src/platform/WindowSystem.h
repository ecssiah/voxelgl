#pragma once

#include <string>

struct GLFWwindow;



class WindowSystem 
{

public:

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

    static constexpr int WINDOW_WIDTH = 1024;
    static constexpr int WINDOW_HEIGHT = 768;

    static bool init();
    static bool create(const char* title);
    static void destroy();

    static bool should_close();
    static void request_close();
    
    static void swap_buffers();
    static void poll_events();

    static void* get_proc_address(const char* name);

    static std::string get_version_info();

    static float get_aspect_ratio();

    static double get_dt();

    static void set_cursor_enabled(bool enabled);

private:

    static GLFWwindow* s_window;

    static double s_last_time;

};