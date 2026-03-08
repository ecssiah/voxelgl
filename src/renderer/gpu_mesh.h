#pragma once

#include <glad/glad.h>
#include <cglm/cglm.h>

#include "core/types.h"

struct GpuMesh
{
    u32 version;

    GLuint vbo_id;
    GLuint ebo_id;

    u32 index_count;

    mat4 model_matrix;
};