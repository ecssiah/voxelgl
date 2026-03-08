#include "action.h"

void action_queue_init(ActionQueue* action_queue, u32 capacity)
{
    action_queue->action_array = (Action*)malloc(sizeof (Action) * capacity);
}

void action_queue_insert(ActionQueue* action_queue, Action action)
{
    action_queue->action_array[action_queue->action_count] = action;
    action_queue->action_count +=1;
}

bool action_queue_is_empty(ActionQueue* action_queue)
{
    return action_queue->action_count <= 0;
}

Action action_queue_pop(ActionQueue* action_queue)
{
    action_queue->action_count -= 1;

    return action_queue->action_array[action_queue->action_count];
}

void action_queue_destroy(ActionQueue* action_queue)
{
    free(action_queue->action_array);
}