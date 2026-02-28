#pragma once

#include "sector.h"

struct World
{
    Sector m_sector_array[WORLD_VOLUME_IN_SECTORS];

    bool init();

    Cell* get_cell(ivec3 grid_position);
    
    void set_block_kind(ivec3 grid_position, BlockKind block_kind);
    void set_block_kind_cube(ivec3 grid_position_min, ivec3 grid_position_max, BlockKind block_kind);
    void set_block_kind_wireframe(ivec3 grid_position_min, ivec3 grid_position_max, BlockKind block_kind);

    void update_cell_face_mask(ivec3 grid_position, Cell* cell);
};