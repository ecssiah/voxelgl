#pragma once

#include "app/world/grid.h"
#include <cglm/cglm.h>

enum CellFace 
{
    CELL_FACE_POS_X,
    CELL_FACE_NEG_X,
    CELL_FACE_POS_Y,
    CELL_FACE_NEG_Y,
    CELL_FACE_POS_Z,
    CELL_FACE_NEG_Z,
    CELL_FACE_COUNT,
};

#define CELL_FACE_BIT(cell_face) (1u << (cell_face))

static ivec3 CELL_FACE_OFFSET_ARRAY[CELL_FACE_COUNT] = {
    {+1, +0, +0},
    {-1, +0, +0},
    {+0, +1, +0},
    {+0, -1, +0},
    {+0, +0, +1},
    {+0, +0, -1},
};

enum BlockKind
{
    BLOCK_KIND_NONE,
    BLOCK_KIND_STONE,
    BLOCK_KIND_EAGLE,
    BLOCK_KIND_LION,
    BLOCK_KIND_WOLF,
    BLOCK_KIND_HORSE,
    BLOCK_KIND_COUNT,
};

static const char* BLOCK_KIND_NAME_ARRAY[BLOCK_KIND_COUNT] =
{
    "None",
    "Stone",
    "Eagle",
    "Lion",
    "Wolf",
    "Horse",
};

struct Cell
{
    CellIndex m_cell_index;
    
    BlockKind m_block_kind;
    uint8_t m_cell_face_mask;

    void init(CellIndex cell_index);
};