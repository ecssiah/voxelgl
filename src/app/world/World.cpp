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

void World::set_block_kind(ivec3 grid_position, BlockKind block_kind)
{
    if (!grid_position_is_valid(grid_position))
    {
        printf("Invalid set position: (%d %d %d)\n", grid_position[0], grid_position[1], grid_position[2]);

        return;
    }

    const SectorIndex sector_index = grid_position_to_sector_index(grid_position);
    const CellIndex cell_index = grid_position_to_cell_index(grid_position);

    Sector* sector = &m_sector_array[sector_index];
    Cell* cell = &sector->m_cell_array[cell_index];

    cell->m_block_kind = block_kind;

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

void World::set_block_kind_wireframe(ivec3 grid_position_min, ivec3 grid_position_max, BlockKind block_kind)
{
    for (int z = grid_position_min[2]; z <= grid_position_max[2]; ++z)
    {
        for (int y = grid_position_min[1]; y <= grid_position_max[1]; ++y)
        {
            for (int x = grid_position_min[0]; x <= grid_position_max[0]; ++x)
            {
                ivec3 grid_position;
                grid_position[0] = x;
                grid_position[1] = y;
                grid_position[2] = z;

                int boundaries = 0;

                if (x == grid_position_min[0] || x == grid_position_max[0]) 
                {
                    boundaries++;
                }

                if (y == grid_position_min[1] || y == grid_position_max[1]) 
                {
                    boundaries++;
                }

                if (z == grid_position_min[2] || z == grid_position_max[2]) 
                {
                    boundaries++;
                }

                if (boundaries >= 2)
                {
                    set_block_kind(grid_position, block_kind);
                }
            }
        }
    }
}

void World::set_block_kind_cube(ivec3 grid_position_min, ivec3 grid_position_max, BlockKind block_kind)
{
    for (int z = grid_position_min[2]; z <= grid_position_max[2]; ++z)
    {
        for (int y = grid_position_min[1]; y <= grid_position_max[1]; ++y)
        {
            for (int x = grid_position_min[0]; x <= grid_position_max[0]; ++x)
            {
                ivec3 grid_position;
                grid_position[0] = x;
                grid_position[1] = y;
                grid_position[2] = z;
                
                set_block_kind(grid_position, block_kind);
            }
        }
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