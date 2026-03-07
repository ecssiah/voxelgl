#pragma once

#include <glad/glad.h>
#include <cglm/cglm.h>

#include "core/types.h"

struct GpuMesh
{
    u32 version = 0;

    GLuint vao_id = 0;
    GLuint vbo_id = 0;
    GLuint ebo_id = 0;

    u32 index_count = 0;

    mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
};