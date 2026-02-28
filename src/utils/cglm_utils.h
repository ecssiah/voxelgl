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

inline void vec3_print(const char* name, vec3 vec)
{
    printf("%s:\n(%f %f %f)\n\n", name, vec[0], vec[1], vec[2]);
}

inline void ivec3_print(const char* name, ivec3 ivec)
{
    printf("%s:\n(%d %d %d)\n\n", name, ivec[0], ivec[1], ivec[2]);
}

inline void mat4_print(const char* name, mat4 mat)
{
    printf("%s:\n", name);

    for (int row = 0; row < 4; ++row)
    {
        printf("%f %f %f %f\n", mat[0][row], mat[1][row], mat[2][row], mat[3][row]);
    }

    printf("\n");
}

}