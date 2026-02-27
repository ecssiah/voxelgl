#pragma once

#include "Sector.h"
#include "metrics.h"

struct World
{

    Sector m_sector_array[WORLD_VOLUME_IN_SECTORS];

    bool init();

    void set_block(ivec3 grid_position, BlockType block_type);

};