#pragma once

#include <vector>
#include <cglm/cglm.h>
#include "vertex_data.h"

struct SectorMesh
{
    uint32_t version = 0;

    vec3 sector_world_position = GLM_VEC3_ZERO_INIT;

    std::vector<VertexData> vertex_vec = {};
    std::vector<uint32_t> index_vec = {};
};