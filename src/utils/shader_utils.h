#pragma once

#include <cstdio>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

namespace shader_utils
{

static std::string load_text_file(const char* filename)
{
    std::ifstream file(filename, std::ios::in);

    if (!file.is_open())
    {
        fprintf(stderr, "[FILE ERROR] Could not open %s\n", filename);

        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

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

        fprintf(stderr, "[SHADER COMPILE ERROR]\n%s\n", log);
    }

    return shader_id;
}

}
