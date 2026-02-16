#include <cglm/cglm.h>
#include <iostream>

namespace cglm_utils
{

inline void normalize_vec3_safe(vec3 out_vector)
{
    if (glm_vec3_norm(out_vector) > 0.0f) 
    {
        glm_vec3_normalize(out_vector);
    }
    else
    {
        out_vector[0] = 0.0f;
        out_vector[1] = 0.0f;
        out_vector[2] = 0.0f;
    }
}

inline void print_vec3(vec3 vector)
{
    std::cout << 
        "vec3: (" << 
        vector[0] << " " << 
        vector[1] << " " << 
        vector[2] << 
        ")" << std::endl;
}

inline void print_mat4(const char* name, mat4 matrix)
{
    std::cout << name << ":\n";
    
    for (int row = 0; row < 4; ++row)
    {
        std::cout
            << matrix[0][row] << " "
            << matrix[1][row] << " "
            << matrix[2][row] << " "
            << matrix[3][row] << "\n";
    }

    std::cout << "\n";
}

}