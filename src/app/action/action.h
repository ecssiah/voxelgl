#pragma once

#include "app/world/world.h"
#include "core/types.h"

enum ActionType
{
    ACTION_PLACE,
    ACTION_REMOVE,
};

struct Action
{
    ActionType action_type;

    union
    {
        struct
        {
            BlockKind block_kind;
            vec3 origin;
            vec3 direction;
        }
        place;

        struct
        {
            vec3 origin;
            vec3 direction;
        } 
        remove;
    };
};

struct ActionQueue
{
    u32 action_capacity;
    u32 action_count;

    Action* action_array;
};

void action_queue_init(ActionQueue* action_queue, u32 capacity);
void action_queue_insert(ActionQueue* action_queue, Action action);
bool action_queue_is_empty(ActionQueue* action_queue);
Action action_queue_pop(ActionQueue* action_queue);
void action_queue_destroy(ActionQueue* action_queue);

