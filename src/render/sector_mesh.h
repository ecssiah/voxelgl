#pragma once

#include <vector>
#include <cglm/cglm.h>
#include "vertex_data.h"

struct SectorMesh
{
    uint32_t m_version = 0;

    vec3 m_sector_world_position;

    std::vector<VertexData> m_vertex_vec;
    std::vector<uint32_t> m_index_vec;
};