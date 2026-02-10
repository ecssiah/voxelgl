#pragma once

#include <cglm/cglm.h>

class Renderer {

public:

    bool start();
    void render(float dt);

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;

    unsigned int m_program = 0;
    unsigned int m_texture_id = 0;

    float m_time = 0.0f;

    static unsigned int load_texture_2d(const char* path);

    void calculate_mvp(mat4& out_mvp);
};