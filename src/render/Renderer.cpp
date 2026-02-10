#include "Renderer.h"
#include "VoxelMesh.h"
#include "gl_utils.h"

#include <stb_image.h>
#include <iostream>
#include <fstream>
#include <sstream>

static GLuint compile_shader(GLuint type, const char* src) 
{
    GLuint shader_id { glCreateShader(type) };

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

static std::string load_text_file(const char* path)
{
    std::ifstream file(path, std::ios::in);

    if (!file.is_open()) 
    {
        std::cerr << "[FILE ERROR] Could not open " << path << "\n";

        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

bool Renderer::start() 
{
    glGenVertexArrays(1, &m_vao);

    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(voxel_vertex_array), 
        voxel_vertex_array, 
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        sizeof(voxel_index_array),
        voxel_index_array, 
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertex),
        (void*)offsetof(VoxelVertex, position_array)
    );

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertex),
        (void*)offsetof(VoxelVertex, normal_array)
    );

    glVertexAttribPointer(
        2, 
        2, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(VoxelVertex),
        (void*)offsetof(VoxelVertex, uv_array)
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    const std::string shader_vert_src { load_text_file("assets/shaders/voxel.vert") };
    const std::string shader_frag_src { load_text_file("assets/shaders/voxel.frag") };

    if (shader_vert_src.empty() || shader_frag_src.empty()) 
    {
        return false;
    }

    const GLuint shader_vert { 
        compile_shader(
            GL_VERTEX_SHADER,
            shader_vert_src.c_str()
        )
    };

    const GLuint shader_frag { 
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

    if (!linked) 
    {
        char log[1024];
        glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);

        std::cerr << "[PROGRAM LINK ERROR]\n" << log << "\n";
    }

    glDeleteShader(shader_vert);
    glDeleteShader(shader_frag);

    m_texture_id = load_texture_2d("assets/textures/lion.png");

    if (m_texture_id == 0) 
    {
        std::cerr << "Texture failed to load\n";

        return false;
    }

    glEnable(GL_DEPTH_TEST);

    return true;
}

void Renderer::render(float dt) 
{
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_time += dt;

    mat4 mvp;
    calculate_mvp(mvp);

    glUseProgram(m_program);

    glUniformMatrix4fv(
        glGetUniformLocation(m_program, "u_mvp"),
        1,
        GL_FALSE,
        (float*)mvp
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    const GLint texture_sampler_id { 
        glGetUniformLocation(m_program, "u_texture_sampler") 
    };

    if (texture_sampler_id != -1) 
    {
        glUniform1i(texture_sampler_id, 0);
    }

    glBindVertexArray(m_vao);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::calculate_mvp(mat4& out_mvp) 
{
    mat4 model;
    mat4 view;
    mat4 projection;

    glm_mat4_identity(model);

    glm_rotate(model, 0.25 * m_time, (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, 0.50 * m_time, (vec3){0.0f, 0.0f, 1.0f});

    glm_mat4_identity(view);
    glm_translate(view, (vec3){0.0f, 0.0f, -2.0f});

    glm_perspective(
        glm_rad(60.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f,
        projection
    );

    glm_mat4_mul(projection, view, out_mvp);
    glm_mat4_mul(out_mvp, model, out_mvp);
}

GLuint Renderer::load_texture_2d(const char* path)
{
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* pixel_data { 
        stbi_load(
            path,
            &width,
            &height,
            &channels,
            4 // force RGBA
        )
    };

    if (!pixel_data) 
    {
        std::cerr << "Failed to load image: " << path << "\n";

        return 0;
    }

    GLuint texture_id = 0;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixel_data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(pixel_data);

    return texture_id;
}