#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>

static char* shader_load(const char* filename)
{
    FILE* file = fopen(filename, "rb");

    if (!file)
    {
        fprintf(stderr, "[FILE ERROR] Could not open %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size + 1);

    if (!buffer)
    {
        fprintf(stderr, "[MEMORY ERROR] Could not allocate shader buffer\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

static GLuint shader_compile(GLuint type_id, const char* src)
{
    GLuint shader_id = glCreateShader(type_id);

    glShaderSource(shader_id, 1, &src, NULL);
    glCompileShader(shader_id);

    int success = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[1024];
        glGetShaderInfoLog(shader_id, sizeof(log), NULL, log);

        fprintf(stderr, "[SHADER COMPILE ERROR]\n%s\n", log);
    }

    return shader_id;
}