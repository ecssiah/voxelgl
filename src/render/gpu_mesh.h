#pragma once

#include <glad/glad.h>
#include <cglm/cglm.h>

struct GpuMesh
{
    uint32_t m_version = 0;

    GLuint m_vao_id = 0;
    GLuint m_vbo_id = 0;
    GLuint m_ebo_id = 0;

    uint32_t m_index_count = 0;

    mat4 m_model_matrix = GLM_MAT4_IDENTITY_INIT;
};