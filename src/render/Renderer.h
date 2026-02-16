#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>



class Renderer
{

public:

    bool start();

    void render(mat4 view_matrix, mat4 projection_matrix);

private:

    void calculate_mvp(mat4 view_matrix, mat4 projection_matrix, mat4 out_mvp_matrix);

    GLuint m_vao_id = 0;
    GLuint m_vbo_id = 0;
    GLuint m_ebo_id = 0;

    GLuint m_program_id = 0;
    GLuint m_texture_id = 0;

    mat4 m_model_matrix;

};