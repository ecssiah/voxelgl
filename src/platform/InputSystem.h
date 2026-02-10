#include <GLFW/glfw3.h>
#include <array>

class InputSystem
{

public:

    static void begin_frame();

    static void on_key(int key, int scancode, int action, int mods);
    static void on_mouse_button(int button, int action, int mods);
    static void on_cursor_position(double xpos, double ypos);

    static bool is_key_pressed(int key);
    static bool is_key_down(int key);

    static bool is_mouse_button_down(int button);
    static bool is_mouse_button_pressed(int button);

    static double mouse_x();
    static double mouse_y();
    static double mouse_dx();
    static double mouse_dy();

private:

    static std::array<bool, GLFW_KEY_LAST + 1> s_current_key_array;
    static std::array<bool, GLFW_KEY_LAST + 1> s_previous_key_array;

    static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> s_current_mouse_array;
    static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> s_previous_mouse_array;

    static double s_current_mouse_x;
    static double s_current_mouse_y;
    static double s_previous_mouse_x;
    static double s_previous_mouse_y;

};