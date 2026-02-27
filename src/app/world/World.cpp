#include "World.h"

#include <stdlib.h>
#include <time.h>

bool World::init()
{
    srand((unsigned int)time(NULL));

    for (SectorIndex sector_index = 0; sector_index < WORLD_VOLUME_IN_SECTORS; sector_index++)
    {
        m_sector_array[sector_index].init(sector_index);
    }

    return true;
}

void World::set_block(ivec3 grid_position, BlockType block_type)
{   
    if (grid_position_is_valid(grid_position))
    {
        const SectorIndex sector_index = grid_position_to_sector_index(grid_position);
        const CellIndex cell_index = grid_position_to_cell_index(grid_position);

        Sector* sector = &m_sector_array[sector_index];

        sector->m_cell_array[cell_index].m_block_type = block_type;
        sector->m_version += 1;
    }
}