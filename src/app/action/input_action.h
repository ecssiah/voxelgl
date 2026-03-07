#include "app/world/cell.h"

enum InputActionType
{
    INPUT_ACTION_PLACE,
    INPUT_ACTION_REMOVE,
};

struct InputAction
{
    InputActionType input_action_type;

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

struct InputActionQueue
{
    uint32_t action_capacity;
    uint32_t action_count;

    InputAction* action_array;

    void init(uint32_t capacity);
    void queue(InputAction input_action);
    bool is_empty();
    InputAction pop();
};

