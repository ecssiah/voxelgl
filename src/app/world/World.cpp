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
        SectorCoordinate sector_coordinate;
        sector_index_to_sector_coordinate(sector_index, sector_coordinate);

        printf("Sector: %d (%d %d %d)\n", sector_index, sector_coordinate[0], sector_coordinate[1], sector_coordinate[2]);

        m_sector_array[sector_index].init(sector_index);
    }

    return true;
}

Cell* World::get_cell(GridCoordinate grid_coordinate)
{
    if (grid_coordinate_is_valid(grid_coordinate))
    {
        const SectorIndex sector_index = grid_coordinate_to_sector_index(grid_coordinate);
        const CellIndex cell_index = grid_coordinate_to_cell_index(grid_coordinate);

        Sector* sector = &m_sector_array[sector_index];

        return &sector->m_cell_array[cell_index];
    }
    else 
    {
        return NULL;
    }
}

void World::set_block_kind(GridCoordinate grid_coordinate, BlockKind block_kind)
{
    if (!grid_coordinate_is_valid(grid_coordinate))
    {
        printf("Invalid set position: (%d %d %d)\n", grid_coordinate[0], grid_coordinate[1], grid_coordinate[2]);

        return;
    }

    const SectorIndex sector_index = grid_coordinate_to_sector_index(grid_coordinate);
    const CellIndex cell_index = grid_coordinate_to_cell_index(grid_coordinate);

    Sector* sector = &m_sector_array[sector_index];
    Cell* cell = &sector->m_cell_array[cell_index];

    cell->m_block_kind = block_kind;

    update_face_exposure(cell, grid_coordinate);

    sector->m_version += 1;
}

void World::set_block_kind_wireframe(GridCoordinate grid_coordinate_min, GridCoordinate grid_coordinate_max, BlockKind block_kind)
{
    for (int z = grid_coordinate_min[2]; z <= grid_coordinate_max[2]; ++z)
    {
        for (int y = grid_coordinate_min[1]; y <= grid_coordinate_max[1]; ++y)
        {
            for (int x = grid_coordinate_min[0]; x <= grid_coordinate_max[0]; ++x)
            {
                GridCoordinate grid_coordinate;
                grid_coordinate[0] = x;
                grid_coordinate[1] = y;
                grid_coordinate[2] = z;

                int boundaries = 0;

                if (x == grid_coordinate_min[0] || x == grid_coordinate_max[0]) 
                {
                    boundaries++;
                }

                if (y == grid_coordinate_min[1] || y == grid_coordinate_max[1]) 
                {
                    boundaries++;
                }

                if (z == grid_coordinate_min[2] || z == grid_coordinate_max[2]) 
                {
                    boundaries++;
                }

                if (boundaries >= 2)
                {
                    set_block_kind(grid_coordinate, block_kind);
                }
            }
        }
    }
}

void World::set_block_kind_cube(GridCoordinate grid_coordinate_min, GridCoordinate grid_coordinate_max, BlockKind block_kind)
{
    for (int z = grid_coordinate_min[2]; z <= grid_coordinate_max[2]; ++z)
    {
        for (int y = grid_coordinate_min[1]; y <= grid_coordinate_max[1]; ++y)
        {
            for (int x = grid_coordinate_min[0]; x <= grid_coordinate_max[0]; ++x)
            {
                GridCoordinate grid_coordinate;
                grid_coordinate[0] = x;
                grid_coordinate[1] = y;
                grid_coordinate[2] = z;

                set_block_kind(grid_coordinate, block_kind);
            }
        }
    }
}

void World::update_face_exposure(Cell* cell, GridCoordinate grid_coordinate)
{
    update_cell_face_mask(cell, grid_coordinate);

    for (int cell_face_index = 0; cell_face_index < CELL_FACE_COUNT; ++cell_face_index)
    {
        GridCoordinate neighbor_grid_coordinate;
        glm_ivec3_add(grid_coordinate, CELL_FACE_OFFSET_ARRAY[cell_face_index], neighbor_grid_coordinate);

        Cell* neighbor_cell = get_cell(neighbor_grid_coordinate);

        if (neighbor_cell)
        {
            update_cell_face_mask(neighbor_cell, neighbor_grid_coordinate);

            const SectorIndex neighbor_sector_index = grid_coordinate_to_sector_index(neighbor_grid_coordinate);
            m_sector_array[neighbor_sector_index].m_version += 1;
        }
    }
}

void World::update_cell_face_mask(Cell* cell, GridCoordinate grid_coordinate)
{
    cell->m_cell_face_mask = 0;

    if (cell->m_block_kind == BLOCK_KIND_NONE)
    {
        return;
    }

    for (int cell_face_index = 0; cell_face_index < CELL_FACE_COUNT; ++cell_face_index)
    {
        GridCoordinate neighbor_grid_coordinate;
        glm_ivec3_add(grid_coordinate, CELL_FACE_OFFSET_ARRAY[cell_face_index], neighbor_grid_coordinate);

        Cell* neighbor_cell = get_cell(neighbor_grid_coordinate);

        if (neighbor_cell == NULL || neighbor_cell->m_block_kind == BLOCK_KIND_NONE)
        {
            cell->m_cell_face_mask |= CELL_FACE_BIT(cell_face_index);
        }
    }
}