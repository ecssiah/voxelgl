#pragma once

struct GLFWwindow;

class WindowSystem {

public:
    static bool init_library();
    static bool create(int width, int height, const char* title);
    static void destroy();

    static bool should_close();
    static void swap_buffers();
    static void poll_events();

    static void* get_proc_address(const char* name);

private:
    static GLFWwindow* s_window;

};