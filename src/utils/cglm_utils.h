#pragma once

#include <cglm/cglm.h>
#include <iostream>


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
    std::cout << name << ":\n";

    std::cout << "(" << vector[0] << " " << vector[1] << " " << vector[2] << ")\n";

    std::cout << "\n";
}

inline void mat4_print(const char* name, mat4 matrix)
{
    std::cout << name << ":\n";
    
    for (int row = 0; row < 4; ++row)
    {
        std::cout << 
            matrix[0][row] << " " << 
            matrix[1][row] << " " << 
            matrix[2][row] << " " << 
            matrix[3][row] << "\n";
    }

    std::cout << "\n";
}

}