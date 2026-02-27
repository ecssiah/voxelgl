#include "world.h"
#include "app/world/cell.h"
#include "app/world/grid.h"
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

Cell* World::get_cell(ivec3 grid_position)
{
    if (grid_position_is_valid(grid_position))
    {
        const SectorIndex sector_index = grid_position_to_sector_index(grid_position);
        const CellIndex cell_index = grid_position_to_cell_index(grid_position);

        Sector* sector = &m_sector_array[sector_index];

        return &sector->m_cell_array[cell_index];
    }
    else 
    {
        return NULL;
    }
}

void World::set_block_kind(ivec3 grid_position, BlockKind block_type)
{
    if (grid_position_is_valid(grid_position))
    {
        const SectorIndex sector_index = grid_position_to_sector_index(grid_position);
        const CellIndex cell_index = grid_position_to_cell_index(grid_position);

        Sector* sector = &m_sector_array[sector_index];
        Cell* cell = &sector->m_cell_array[cell_index];

        cell->m_block_kind = block_type;

        update_cell_face_mask(grid_position, cell);

        for (int cell_face = 0; cell_face < CELL_FACE_COUNT; ++cell_face)
        {
            ivec3 neighbor_grid_position;
            glm_ivec3_add(grid_position, CELL_FACE_OFFSET_ARRAY[cell_face], neighbor_grid_position);

            Cell* neighbor_cell = get_cell(neighbor_grid_position);

            if (neighbor_cell)
            {
                update_cell_face_mask(neighbor_grid_position, neighbor_cell);

                const SectorIndex neighbor_sector_index = grid_position_to_sector_index(neighbor_grid_position);
                m_sector_array[neighbor_sector_index].m_version += 1;
            }
        }

        sector->m_version += 1;
    }
}

void World::update_cell_face_mask(ivec3 grid_position, Cell* cell)
{
    cell->m_cell_face_mask = 0;

    if (cell->m_block_kind == BLOCK_KIND_NONE)
    {
        return;
    }

    for (int cell_face = 0; cell_face < CELL_FACE_COUNT; ++cell_face)
    {
        ivec3 neighbor_grid_position;
        glm_ivec3_add(grid_position, CELL_FACE_OFFSET_ARRAY[cell_face], neighbor_grid_position);

        Cell* neighbor_cell = get_cell(neighbor_grid_position);

        if (neighbor_cell == NULL || neighbor_cell->m_block_kind == BLOCK_KIND_NONE)
        {
            cell->m_cell_face_mask |= static_cast<uint8_t>(1u << cell_face);
        }
    }
}