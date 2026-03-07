#pragma once

#include <cstdio>

#include <glad/glad.h>

static void gl_ext_get_errors(const char* label) 
{
    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR) 
    {
        printf("[GL ERROR] : 0x%X\n", err);
    }
}