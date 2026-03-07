#pragma once

#include <cglm/cglm.h>

static void cglm_ext_vec3_normalize_safe(vec3 out_vector)
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