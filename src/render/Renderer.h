#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

class Renderer
{

public:

    bool start();

    void render(mat4 view_matrix, mat4 projection_matrix);

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    GLuint m_program = 0;
    GLuint m_texture_id = 0;

    mat4 m_model_matrix;

    static GLuint load_texture_2d(const char* path);

    void calculate_mvp(mat4 view_matrix, mat4 projection_matrix, mat4 out_mvp_matrix);
};