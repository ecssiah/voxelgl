#pragma once

#include <vector>
#include <cglm/cglm.h>
#include "vertex_data.h"

static vec3 FACE_VERTEX_ARRAY[6][4] =
{
    // +X (x = +0.5)
    {
        {+0.5f, -0.5f, -0.5f },
        {+0.5f, -0.5f, +0.5f },
        {+0.5f, +0.5f, +0.5f },
        {+0.5f, +0.5f, -0.5f }
    },

    // -X (x = -0.5)
    {
        { -0.5f, -0.5f, -0.5f },
        { -0.5f, +0.5f, -0.5f },
        { -0.5f, +0.5f, +0.5f },
        { -0.5f, -0.5f, +0.5f }
    },

    // +Y (y = +0.5)
    {
        { -0.5f, +0.5f, -0.5f },
        { +0.5f, +0.5f, -0.5f },
        { +0.5f, +0.5f, +0.5f },
        { -0.5f, +0.5f, +0.5f }
    },

    // -Y (y = -0.5)
    {
        { -0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f, +0.5f },
        { +0.5f, -0.5f, +0.5f },
        { +0.5f, -0.5f, -0.5f }
    },

    // +Z (z = +0.5)
    {
        { -0.5f, -0.5f, +0.5f },
        { -0.5f, +0.5f, +0.5f },
        { +0.5f, +0.5f, +0.5f },
        { +0.5f, -0.5f, +0.5f }
    },

    // -Z (z = -0.5)
    {
        { -0.5f, -0.5f, -0.5f },
        { +0.5f, -0.5f, -0.5f },
        { +0.5f, +0.5f, -0.5f },
        { -0.5f, +0.5f, -0.5f }
    },
};

struct SectorMesh
{
    uint32_t m_version = 0;

    vec3 m_sector_world_position;

    std::vector<VertexData> m_vertex_vec;
    std::vector<uint32_t> m_index_vec;
};