#pragma once

#include <vector>

#include "VertexData.h"

struct SectorMesh
{

    uint32_t m_version = 0;

    std::vector<VertexData> m_vertex_vec;
    std::vector<uint32_t> m_index_vec;

};