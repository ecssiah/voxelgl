#pragma once

#include <cglm/cglm.h>

#include "metrics.h"

enum BlockType {
    None,
    Eagle,
    Lion,
    Wolf,
    Horse,
    Count,
};

static const char* BlockTypeNames[BlockType::Count] =
{
    "None",
    "Eagle",
    "Lion",
    "Wolf",
    "Horse",
};

struct Cell
{
    void init(CellIndex cell_index);
    
    CellIndex m_cell_index;

    BlockType m_block_type;
};