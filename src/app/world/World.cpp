#include "world.h"
#include "app/world/cell.h"
#include "app/world/grid.h"
#include "platform/input_system.h"

#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <time.h>

#include <GLFW/glfw3.h>

bool World::init()
{
    srand((unsigned int)time(NULL));

    for (SectorIndex sector_index = 0; sector_index < WORLD_VOLUME_IN_SECTORS; sector_index++)
    {
        SectorCoordinate sector_coordinate;
        sector_index_to_sector_coordinate(sector_index, sector_coordinate);

        m_sector_array[sector_index].init(sector_index);
    }

    return true;
}

void World::update(float dt)
{
    while (!InputSystem::input_action_deque.empty())
    {
        InputAction input_action = InputSystem::input_action_deque.front();
        InputSystem::input_action_deque.pop_front();

        switch (input_action.input_action_type)
        {
        case INPUT_ACTION_PLACE:
        {
            Cell* place_hit_cell = line_trace(input_action.place.origin, input_action.place.direction);

            if (place_hit_cell)
            {
                GridCoordinate grid_coordinate;
    
                grid_coordinate[0] = (int)floorf(input_action.place.origin[0]);
                grid_coordinate[1] = (int)floorf(input_action.place.origin[1]);
                grid_coordinate[2] = (int)floorf(input_action.place.origin[2]);

                set_block_kind(grid_coordinate, BLOCK_KIND_WOLF);
            }

            break;
        }
        case INPUT_ACTION_REMOVE:
        {
            Cell* remove_hit_cell = line_trace(input_action.remove.origin, input_action.remove.direction);
            
            if (remove_hit_cell)
            {
                GridCoordinate grid_coordinate;
    
                grid_coordinate[0] = (int)floorf(input_action.remove.origin[0]);
                grid_coordinate[1] = (int)floorf(input_action.remove.origin[1]);
                grid_coordinate[2] = (int)floorf(input_action.remove.origin[2]);

                set_block_kind(grid_coordinate, BLOCK_KIND_NONE);
            }

            break;
        }
        default:
            break;
        }
    }
}

Cell* World::line_trace(vec3 origin, vec3 direction)
{
    glm_vec3_print(origin, stdout);
    glm_vec3_print(direction, stdout);

    GridCoordinate grid_coordinate;

    grid_coordinate[0] = (int)floorf(origin[0]);
    grid_coordinate[1] = (int)floorf(origin[1]);
    grid_coordinate[2] = (int)floorf(origin[2]);

    int stepX = (direction[0] > 0) ? 1 : -1;
    int stepY = (direction[1] > 0) ? 1 : -1;
    int stepZ = (direction[2] > 0) ? 1 : -1;

    float tDeltaX = fabsf(1.0f / direction[0]);
    float tDeltaY = fabsf(1.0f / direction[1]);
    float tDeltaZ = fabsf(1.0f / direction[2]);

    float nextVoxelBoundaryX = grid_coordinate[0] + (stepX > 0 ? 1.0f : 0.0f);
    float nextVoxelBoundaryY = grid_coordinate[1] + (stepY > 0 ? 1.0f : 0.0f);
    float nextVoxelBoundaryZ = grid_coordinate[2] + (stepZ > 0 ? 1.0f : 0.0f);

    float tMaxX = (nextVoxelBoundaryX - origin[0]) / direction[0];
    float tMaxY = (nextVoxelBoundaryY - origin[1]) / direction[1];
    float tMaxZ = (nextVoxelBoundaryZ - origin[2]) / direction[2];

    const int MAX_STEPS = 128;

    for (int i = 0; i < MAX_STEPS; i++)
    {
        Cell* cell = get_cell(grid_coordinate);

        if (cell && cell->m_block_kind != BLOCK_KIND_NONE)
        {
            return cell;
        }

        if (tMaxX < tMaxY)
        {
            if (tMaxX < tMaxZ)
            {
                grid_coordinate[0] += stepX;
                tMaxX += tDeltaX;
            }
            else
            {
                grid_coordinate[2] += stepZ;
                tMaxZ += tDeltaZ;
            }
        }
        else
        {
            if (tMaxY < tMaxZ)
            {
                grid_coordinate[1] += stepY;
                tMaxY += tDeltaY;
            }
            else
            {
                grid_coordinate[2] += stepZ;
                tMaxZ += tDeltaZ;
            }
        }

        if (!grid_coordinate_is_valid(grid_coordinate))
        {
            break;
        }
    }

    return NULL;
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