#pragma once

#include <GLFW/glfw3.h>

#include "app/action/action.h"
#include "app/world/world.h"

#define MAX_ACTIONS_PER_FRAME 100

struct Input
{
    ActionQueue action_queue;

    bool current_key_array[GLFW_KEY_LAST + 1];
    bool previous_key_array[GLFW_KEY_LAST + 1];
    bool current_mouse_array[GLFW_MOUSE_BUTTON_LAST + 1];
    bool previous_mouse_array[GLFW_MOUSE_BUTTON_LAST + 1];

    f64 current_mouse_x;
    f64 current_mouse_y;
    f64 previous_mouse_x;
    f64 previous_mouse_y;
};

Input* input_create();
void input_init(Input* input);
void input_destroy(Input* input);

void input_begin_frame(Input* input);
void input_handle_key(Input* input, int key, int scancode, int action, int mods);
void input_handle_mouse_button(Input* input, int button, int action, int mods);
void input_handle_cursor_position(Input* input, f64 xpos, f64 ypos);

bool input_is_key_pressed(Input* input, int key);
bool input_is_key_released(Input* input, int key);
bool input_is_key_down(Input* input, int key);

bool input_is_mouse_button_down(Input* input, int button);
bool input_is_mouse_button_released(Input* input, int button);
bool input_is_mouse_button_pressed(Input* input, int button);

f64 input_get_mouse_x(Input* input);
f64 input_get_mouse_y(Input* input);
f64 input_get_mouse_dx(Input* input);
f64 input_get_mouse_dy(Input* input);