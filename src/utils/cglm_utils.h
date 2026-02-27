#pragma once

#include <cglm/cglm.h>

namespace cglm_utils
{

inline void vec3_normalize_safe(vec3 out_vector)
{
    if (glm_vec3_norm(out_vector) > 0.0f) 
    {
        glm_vec3_normalize(out_vector);
    }
    else
    {
        glm_vec3_zero(out_vector);
    }
}

inline void vec3_print(const char* name, vec3 vector)
{
    printf("%s:\n(%f %f %f)\n\n", name, vector[0], vector[1], vector[2]);
}

inline void ivec3_print(const char* name, ivec3 vector)
{
    printf("%s:\n(%d %d %d)\n\n", name, vector[0], vector[1], vector[2]);
}

inline void mat4_print(const char* name, mat4 matrix)
{
    printf("%s:\n", name);

    for (int row = 0; row < 4; ++row)
    {
        printf("%f %f %f %f\n", matrix[0][row], matrix[1][row], matrix[2][row], matrix[3][row]);
    }

    printf("\n");
}

}