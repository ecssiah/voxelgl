#include "platform/InputSystem.h"

std::array<bool, GLFW_KEY_LAST + 1> InputSystem::s_current_key_array;
std::array<bool, GLFW_KEY_LAST + 1> InputSystem::s_previous_key_array;

std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> InputSystem::s_current_mouse_array;
std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> InputSystem::s_previous_mouse_array;

double InputSystem::s_current_mouse_x = 0.0;
double InputSystem::s_current_mouse_y = 0.0;
double InputSystem::s_previous_mouse_x = 0.0;
double InputSystem::s_previous_mouse_y = 0.0;

void InputSystem::begin_frame()
{
    s_previous_key_array = s_current_key_array;
    s_previous_mouse_array = s_current_mouse_array;

    s_previous_mouse_x = s_current_mouse_x;
    s_previous_mouse_y = s_current_mouse_y;
}

void InputSystem::on_key(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        s_current_key_array[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        s_current_key_array[key] = false;
    }
}

void InputSystem::on_mouse_button(int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        s_current_mouse_array[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        s_current_mouse_array[button] = false;
    }
}

void InputSystem::on_cursor_position(double xpos, double ypos)
{
    s_current_mouse_x = xpos;
    s_current_mouse_y = ypos;
}

bool InputSystem::is_key_down(int key)
{
    return s_current_key_array[key];
}

bool InputSystem::is_key_pressed(int key)
{
    return s_current_key_array[key] && !s_previous_key_array[key];
}

double InputSystem::get_mouse_x()
{
    return s_current_mouse_x;
}

double InputSystem::get_mouse_y()
{
    return s_current_mouse_y;
}

double InputSystem::get_mouse_dx()
{
    const double dx = s_current_mouse_x - s_previous_mouse_x;

    if (dx > 50.0)
    {
        return 0.0;
    }
    else
    {
        return s_current_mouse_x - s_previous_mouse_x;
    }
}

double InputSystem::get_mouse_dy()
{
    const double dy = s_current_mouse_y - s_previous_mouse_y;

    if (dy > 50.0)
    {
        return 0.0;
    }
    else
    {
        return s_current_mouse_y - s_previous_mouse_y;
    }
}

