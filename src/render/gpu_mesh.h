#pragma once

#include <glad/glad.h>
#include <cglm/cglm.h>

struct GpuMesh
{
    uint32_t m_version;

    GLuint m_vao_id;
    GLuint m_vbo_id;
    GLuint m_ebo_id;

    uint32_t m_index_count;

    mat4 m_model_matrix;
};