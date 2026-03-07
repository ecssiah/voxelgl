#include "platform/input.h"

Input* input_create()
{
    return (Input*)malloc(sizeof (Input));
}

void input_init(Input* input)
{
    for (u32 i = 0; i < GLFW_KEY_LAST + 1; ++i)
    {
        input->current_key_array[i] = false;
        input->previous_key_array[i] = false;
    }

    for (u32 i = 0; i < GLFW_MOUSE_BUTTON_LAST + 1; ++i)
    {
        input->current_mouse_array[i] = false;
        input->previous_mouse_array[i] = false;
    }

    input->current_mouse_x = 0.0;
    input->current_mouse_y = 0.0;
    input->previous_mouse_x = 0.0;
    input->previous_mouse_y = 0.0;

    action_queue_init(&input->action_queue, MAX_ACTIONS_PER_FRAME);
}

void input_begin_frame(Input* input)
{
    for (u32 i = 0; i < GLFW_KEY_LAST + 1; ++i)
    {
        input->previous_key_array[i] = input->current_key_array[i];
    }

    for (u32 i = 0; i < GLFW_MOUSE_BUTTON_LAST + 1; ++i)
    {
        input->previous_mouse_array[i] = input->current_mouse_array[i];
    }

    input->previous_mouse_x = input->current_mouse_x;
    input->previous_mouse_y = input->current_mouse_y;
}

void input_handle_key(Input* input, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        input->current_key_array[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->current_key_array[key] = false;
    }
}

void input_handle_mouse_button(Input* input, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        input->current_mouse_array[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->current_mouse_array[button] = false;
    }
}

void input_handle_cursor_position(Input* input, f64 xpos, f64 ypos)
{
    input->current_mouse_x = xpos;
    input->current_mouse_y = ypos;
}

bool input_is_key_down(Input* input, int key)
{
    return input->current_key_array[key];
}

bool input_is_key_pressed(Input* input, int key)
{
    return input->current_key_array[key] && !input->previous_key_array[key];
}

bool input_is_key_released(Input* input, int key)
{
    return !input->current_key_array[key] && input->previous_key_array[key];
}

bool input_is_mouse_button_down(Input* input, int button)
{
    return input->current_mouse_array[button];
}

bool input_is_mouse_button_pressed(Input* input, int button)
{
    return input->current_mouse_array[button] && !input->previous_mouse_array[button];
}

bool input_is_mouse_button_released(Input* input, int button)
{
    return !input->current_mouse_array[button] && input->previous_mouse_array[button];
}

f64 input_get_mouse_x(Input* input)
{
    return input->current_mouse_x;
}

f64 input_get_mouse_y(Input* input)
{
    return input->current_mouse_y;
}

f64 input_get_mouse_dx(Input* input)
{
    f64 dx = input->current_mouse_x - input->previous_mouse_x;

    return dx > 50.0 ? 0.0 : dx;
}

f64 input_get_mouse_dy(Input* input)
{
    f64 dy = input->current_mouse_y - input->previous_mouse_y;

    return dy > 50.0 ? 0.0 : dy;
}

