#pragma once

#include <cglm/cglm.h>

#include "core/types.h"

static float quad_vertex_array[] =
{
    // Position           // UV
    -0.5f, -0.5f, +0.0f, +0.0f,
    +0.5f, -0.5f, +1.0f, +0.0f,
    +0.5f, +0.5f, +1.0f, +1.0f,

    -0.5f, -0.5f, +0.0f, +0.0f,
    +0.5f, +0.5f, +1.0f, +1.0f,
    -0.5f, +0.5f, +0.0f, +1.0f
};

struct InterfaceImage
{
    vec2 position;
    vec2 size;
    u8 texture_index;
};

struct Interface
{
    u32 version;
    InterfaceImage* interface_image_array;
    u32 interface_image_count;
    u32 interface_image_capacity;
};

Interface* interface_create(void);
void interface_init(Interface* interface);
void interface_update(Interface* interface);
