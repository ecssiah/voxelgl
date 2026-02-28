#include "app/world/cell.h"

const uint8_t FACES_PER_VOXEL = 6;
const uint8_t VERTICES_PER_FACE = 4;
const uint8_t BLOCK_TEXTURE_SIZE = 64;
const float BLOCK_RADIUS = 0.5f;

static vec3 FACE_VERTEX_ARRAY[FACES_PER_VOXEL][VERTICES_PER_FACE] =
{
    // +X
    {
        {+BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS},
        {+BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS},
        {+BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS},
        {+BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS}
    },

    // -X
    {
        { -BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS},
        { -BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS},
        { -BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS},
        { -BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS}
    },

    // +Y
    {
        { -BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS},
        { +BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS},
        { +BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS},
        { -BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS}
    },

    // -Y
    {
        { -BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS},
        { -BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS},
        { +BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS},
        { +BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS}
    },

    // +Z
    {
        { -BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS},
        { -BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS},
        { +BLOCK_RADIUS, +BLOCK_RADIUS, +BLOCK_RADIUS},
        { +BLOCK_RADIUS, -BLOCK_RADIUS, +BLOCK_RADIUS}
    },

    // -Z
    {
        { -BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS},
        { +BLOCK_RADIUS, -BLOCK_RADIUS, -BLOCK_RADIUS},
        { +BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS},
        { -BLOCK_RADIUS, +BLOCK_RADIUS, -BLOCK_RADIUS}
    },
};

static vec3 FACE_NORMAL_ARRAY[FACES_PER_VOXEL] =
{
    // +X
    {+1, +0, +0},

    // -X
    {-1, +0, +0},

    // +Y
    {+0, +1, +0},

    // -Y
    {+0, -1, +0},  

    // +Z
    {+0, +0, +1},  

    // -Z
    {+0, +0, -1},  
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

static const char* BLOCK_KIND_TEXTURE_FILENAME[BLOCK_KIND_COUNT] =
{
    // BLOCK_KIND_NONE
    NULL,

    // BLOCK_KIND_STONE
    "stone.png",

    // BLOCK_KIND_EAGLE
    "eagle.png",

    // BLOCK_KIND_LION
    "lion.png",

    // BLOCK_KIND_WOLF
    "wolf.png",

    // BLOCK_KIND_HORSE
    "horse.png",
};