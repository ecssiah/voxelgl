#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

class Renderer
{

public:

    bool start();
    void render(float dt);

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    GLuint m_program = 0;
    GLuint m_texture_id = 0;

    float m_time = 0.0f;

    static GLuint load_texture_2d(const char* path);

    void calculate_mvp(mat4& out_mvp);
};