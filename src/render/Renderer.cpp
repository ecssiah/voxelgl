#include "Renderer.h"
#include "VoxelMesh.h"

#include <glad/glad.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

static void check_gl_error(const char* label) 
{
    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << 
            "[GL ERROR] " << label << ": 0x" << 
            std::hex << err << std::dec << "\n";
    }
}

static unsigned int compile_shader(unsigned int type, const char* src) 
{
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

        std::cerr << "[SHADER COMPILE ERROR]\n" << log << "\n";
    }

    return shader;
}

static std::string load_text_file(const char* path)
{
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "[FILE ERROR] Could not open " << path << "\n";
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Renderer::start() 
{
    // --- cube buffers ---
    glGenVertexArrays(1, &m_vao);

    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(voxel_vertex_array), voxel_vertex_array, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(voxel_index_array), voxel_index_array, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    std::string shader_vert_src { load_text_file("assets/shaders/voxel.vert") };
    std::string shader_frag_src { load_text_file("assets/shaders/voxel.frag") };

    if (shader_vert_src.empty() || shader_frag_src.empty()) {
        return false;
    }

    unsigned int shader_vert { 
        compile_shader(
            GL_VERTEX_SHADER,
            shader_vert_src.c_str()
        )
    };

    unsigned int shader_frag { 
        compile_shader(
            GL_FRAGMENT_SHADER,
            shader_frag_src.c_str()
        )
    };

    m_program = glCreateProgram();

    glAttachShader(m_program, shader_vert);
    glAttachShader(m_program, shader_frag);

    glLinkProgram(m_program);

    int linked = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

    if (!linked) {
        char log[1024];
        glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);

        std::cerr << "[PROGRAM LINK ERROR]\n" << log << "\n";
    }

    glDeleteShader(shader_vert);
    glDeleteShader(shader_frag);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void Renderer::render(float dt) 
{
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_time += dt;

    float angle = m_time;

    float c = cos(angle);
    float s = sin(angle);

    mat4 mvp;
    calculate_mvp(mvp);

    glUseProgram(m_program);

    glUniformMatrix4fv(
        glGetUniformLocation(m_program, "uMVP"),
        1,
        GL_FALSE,
        (float*)mvp
    );

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::calculate_mvp(mat4& out_mvp) 
{
    mat4 model;
    mat4 view;
    mat4 projection;

    // model: rotate cube
    glm_mat4_identity(model);
    glm_rotate(model, m_time, (vec3){0.0f, 1.0f, 0.0f});

    // view: move camera back
    glm_mat4_identity(view);
    glm_translate(view, (vec3){0.0f, 0.0f, -2.0f});

    // projection: perspective
    glm_perspective(
        glm_rad(60.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f,
        projection
    );

    // MVP = projection * view * model
    glm_mat4_mul(projection, view, out_mvp);
    glm_mat4_mul(out_mvp, model, out_mvp);
}