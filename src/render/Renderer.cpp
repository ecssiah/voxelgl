#include "Renderer.h"
#include "VoxelData.h"
#include "utils/file_utils.h"
#include "utils/stb_utils.h"
#include <iostream>

static GLuint compile_shader(GLuint type_id, const char* src) 
{
    GLuint shader_id = glCreateShader(type_id);

    glShaderSource(shader_id, 1, &src, nullptr);
    glCompileShader(shader_id);

    int success = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success) 
    {
        char log[1024];
        glGetShaderInfoLog(shader_id, sizeof(log), nullptr, log);

        std::cerr << "[SHADER COMPILE ERROR]\n" << log << "\n";
    }

    return shader_id;
}

bool Renderer::start() 
{
    glGenVertexArrays(1, &m_vao_id);

    glGenBuffers(1, &m_vbo_id);
    glGenBuffers(1, &m_ebo_id);

    glBindVertexArray(m_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);

    glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(VOXEL_VERTEX_ARRAY), 
        VOXEL_VERTEX_ARRAY, 
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        sizeof(VOXEL_INDEX_ARRAY),
        VOXEL_INDEX_ARRAY, 
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertexData),
        (void*)offsetof(VoxelVertexData, position_array)
    );

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertexData),
        (void*)offsetof(VoxelVertexData, normal_array)
    );

    glVertexAttribPointer(
        2, 
        2, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(VoxelVertexData),
        (void*)offsetof(VoxelVertexData, uv_array)
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    const std::string shader_vert_src = file_utils::load_text_file("assets/shaders/voxel.vert");
    const std::string shader_frag_src = file_utils::load_text_file("assets/shaders/voxel.frag");

    if (shader_vert_src.empty() || shader_frag_src.empty()) 
    {
        return false;
    }

    const GLuint shader_vert = 
        compile_shader(
            GL_VERTEX_SHADER,
            shader_vert_src.c_str()
        );

    const GLuint shader_frag =
        compile_shader(
            GL_FRAGMENT_SHADER,
            shader_frag_src.c_str()
        );

    m_program_id = glCreateProgram();

    glAttachShader(m_program_id, shader_vert);
    glAttachShader(m_program_id, shader_frag);

    glLinkProgram(m_program_id);

    int linked = 0;
    glGetProgramiv(m_program_id, GL_LINK_STATUS, &linked);

    if (!linked) 
    {
        char log[1024];
        glGetProgramInfoLog(m_program_id, sizeof(log), nullptr, log);

        std::cerr << "[PROGRAM LINK ERROR]\n" << log << "\n";
    }

    glDeleteShader(shader_vert);
    glDeleteShader(shader_frag);

    m_texture_id = stb_utils::load_texture_2d("assets/textures/lion.png");

    if (m_texture_id == 0) 
    {
        std::cerr << "Texture failed to load\n";

        return false;
    }

    glEnable(GL_DEPTH_TEST);

    glm_mat4_identity(m_model_matrix);

    return true;
}

void Renderer::render(mat4 view_matrix, mat4 projection_matrix) 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float angle = 0.005f;
    vec3 axis = { 0.0f, 1.0f, 1.0f };

    glm_rotate(m_model_matrix, angle, axis);

    mat4 mvp_matrix;
    calculate_mvp(view_matrix, projection_matrix, mvp_matrix);

    glUseProgram(m_program_id);

    glUniformMatrix4fv(
        glGetUniformLocation(m_program_id, "u_mvp_matrix"),
        1,
        GL_FALSE,
        (float*)mvp_matrix
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    const GLint texture_sampler_id = glGetUniformLocation(m_program_id, "u_texture_sampler");

    if (texture_sampler_id != -1) 
    {
        glUniform1i(texture_sampler_id, 0);
    }

    glBindVertexArray(m_vao_id);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::calculate_mvp(
    mat4 view_matrix,
    mat4 projection_matrix,
    mat4 out_mvp_matrix
) 
{
    mat4 view_projection_matrix;
    glm_mat4_mul(projection_matrix, view_matrix, view_projection_matrix);

    glm_mat4_mul(view_projection_matrix, m_model_matrix, out_mvp_matrix);
}
