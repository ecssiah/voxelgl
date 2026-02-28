#pragma once

#include <vector>
#include <cglm/cglm.h>
#include "vertex_data.h"

static constexpr int FACES_PER_VOXEL = 6;
static constexpr int VERTICES_PER_FACE = 4;
static constexpr float BLOCK_RADIUS = 0.5f;

static vec3 FACE_VERTEX_ARRAY[FACES_PER_VOXEL][VERTICES_PER_FACE] =
{
    // +X
    {
        {+BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS },
        {+BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS },
        {+BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS },
        {+BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS }
    },

    // -X
    {
        { -BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS },
        { -BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS },
        { -BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS },
        { -BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS }
    },

    // +Y
    {
        { -BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS },
        { +BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS },
        { +BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS },
        { -BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS }
    },

    // -Y
    {
        { -BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS },
        { -BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS },
        { +BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS },
        { +BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS }
    },

    // +Z
    {
        { -BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS },
        { -BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS },
        { +BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS },
        { +BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS }
    },

    // -Z
    {
        { -BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS },
        { +BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS },
        { +BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS },
        { -BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS }
    },
};

static vec3 FACE_NORMAL_ARRAY[FACES_PER_VOXEL] =
{
    // +X
    { +1, +0, +0 },

    // -X
    { -1, +0, +0 },

    // +Y
    { +0, +1, +0 },

    // -Y
    { +0, -1, +0 },  

    // +Z
    { +0, +0, +1 },  

    // -Z
    { +0, +0, -1 },  
};

static vec2 FACE_UV_ARRAY[FACES_PER_VOXEL][VERTICES_PER_FACE] =
{
    // +X  (Z vertical, Y horizontal)
    {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
    },

    // -X  (Z vertical, Y horizontal)
    {
        {1, 0},
        {1, 1},
        {0, 1},
        {0, 0},
    },

    // +Y  (Z vertical, X horizontal)
    {
        {1, 1},
        {0, 1},
        {0, 0},
        {1, 0},
    },

    // -Y  (Z vertical, X horizontal)
    {
        {1, 0},
        {1, 1},
        {0, 1},
        {0, 0},
    },

    // +Z (top face)
    {
        {0, 0},
        {0, 1},
        {1, 1},
        {1, 0},
    },

    // -Z (bottom face)
    {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
    },
};

struct SectorMesh
{
    uint32_t m_version = 0;

    vec3 m_sector_world_position;

    std::vector<VertexData> m_vertex_vec;
    std::vector<uint32_t> m_index_vec;
};