#include "input_action.h"

void InputActionQueue::init(uint32_t capacity)
{
    action_array = (InputAction*)malloc(sizeof (InputAction) * capacity);
}

void InputActionQueue::queue(InputAction input_action)
{
    action_array[action_count] = input_action;
    action_count +=1;
}

bool InputActionQueue::is_empty()
{
    return action_count <= 0;
}

InputAction InputActionQueue::pop()
{
    action_count -= 1;

    return action_array[action_count];
}