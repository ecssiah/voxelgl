#pragma once



static constexpr float VOXEL_RADIUS = 0.5f;

struct VoxelVertexData 
{
    float position_array[3];
    float normal_array[3];
    float uv_array[2];
};

static constexpr VoxelVertexData VOXEL_VERTEX_ARRAY[] = {
    {
        { -VOXEL_RADIUS, -VOXEL_RADIUS, +VOXEL_RADIUS },
        { 0, 0, 1 },
        { 0, 0 },
    },

    {
        { +VOXEL_RADIUS, -VOXEL_RADIUS, +VOXEL_RADIUS },
        { 0, 0, 1 },
        { 1, 0 },
    },

    {
        { +VOXEL_RADIUS, +VOXEL_RADIUS, +VOXEL_RADIUS },
        { 0, 0, 1 },
        { 1, 1 },
    },

    {
        { -VOXEL_RADIUS, +VOXEL_RADIUS, +VOXEL_RADIUS },
        { 0, 0, 1 },
        { 0, 1 },
    },

    {
        { +VOXEL_RADIUS, -VOXEL_RADIUS, -VOXEL_RADIUS },
        { 0, 0, -1 },
        { 0, 0 },
    },

    {
        { -VOXEL_RADIUS, -VOXEL_RADIUS, -VOXEL_RADIUS },
        { 0, 0, -1 },
        { 1, 0 },
    },

    {
        { -VOXEL_RADIUS, +VOXEL_RADIUS, -VOXEL_RADIUS },
        { 0, 0, -1 },
        { 1, 1 },
    },

    {
        { +VOXEL_RADIUS, +VOXEL_RADIUS, -VOXEL_RADIUS },
        { 0, 0, -1 },
        { 0, 1 },
    },

    {
        { +VOXEL_RADIUS, -VOXEL_RADIUS, +VOXEL_RADIUS },
        { 1, 0, 0 },
        { 0, 0 },
    },

    {
        { +VOXEL_RADIUS, -VOXEL_RADIUS, -VOXEL_RADIUS },
        { 1, 0, 0 },
        { 1, 0 },
    },

    {
        { +VOXEL_RADIUS, +VOXEL_RADIUS, -VOXEL_RADIUS },
        { 1, 0, 0 },
        { 1, 1 },
    },

    {
        { +VOXEL_RADIUS, +VOXEL_RADIUS, +VOXEL_RADIUS },
        { 1, 0, 0 },
        { 0, 1 },
    },

    {
        { -VOXEL_RADIUS, -VOXEL_RADIUS, -VOXEL_RADIUS },
        { -1, 0, 0 },
        { 0, 0 },
    },

    {
        { -VOXEL_RADIUS, -VOXEL_RADIUS, +VOXEL_RADIUS },
        { -1, 0, 0 },
        { 1, 0 },
    },

    {
        { -VOXEL_RADIUS, +VOXEL_RADIUS, +VOXEL_RADIUS },
        { -1, 0, 0 },
        { 1, 1 },
    },

    {
        { -VOXEL_RADIUS, +VOXEL_RADIUS, -VOXEL_RADIUS },
        { -1, 0, 0 },
        { 0, 1 },
    },

    {
        { -VOXEL_RADIUS, +VOXEL_RADIUS, +VOXEL_RADIUS },
        { 0, 1, 0 },
        { 0, 0 },
    },

    {
        { +VOXEL_RADIUS, +VOXEL_RADIUS, +VOXEL_RADIUS },
        { 0, 1, 0 },
        { 1, 0 },
    },

    {
        { +VOXEL_RADIUS, +VOXEL_RADIUS, -VOXEL_RADIUS },
        { 0, 1, 0 },
        { 1, 1 },
    },

    {
        { -VOXEL_RADIUS, +VOXEL_RADIUS, -VOXEL_RADIUS },
        { 0, 1, 0 },
        { 0, 1 },
    },

    {
        { -VOXEL_RADIUS, -VOXEL_RADIUS, -VOXEL_RADIUS },
        { 0, -1, 0 },
        { 0, 0 },
    },

    {
        { +VOXEL_RADIUS, -VOXEL_RADIUS, -VOXEL_RADIUS },
        { 0, -1, 0 },
        { 1, 0 },
    },

    {
        { +VOXEL_RADIUS, -VOXEL_RADIUS, +VOXEL_RADIUS },
        { 0, -1, 0 },
        { 1, 1 },
    },

    {
        { -VOXEL_RADIUS, -VOXEL_RADIUS, +VOXEL_RADIUS },
        { 0, -1, 0 },
        { 0, 1 },
    },
};

static constexpr unsigned int VOXEL_INDEX_ARRAY[] = {
    0, 1, 2, 2,  3, 0, 
    4, 5, 6, 6, 7, 4,  
    8, 9, 10, 10, 11, 8, 
    12, 13, 14, 14, 15, 12, 
    16, 17, 18, 18, 19, 16, 
    20, 21, 22, 22, 23, 20,
};