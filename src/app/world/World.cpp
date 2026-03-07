#include "world.h"
#include "app/world/cell.h"
#include "app/world/grid.h"
#include "platform/input_system.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cfloat>
#include <stdlib.h>
#include <time.h>

#include <GLFW/glfw3.h>

bool World::init()
{
    srand((unsigned int)time(NULL));

    for (SectorIndex sector_index = 0; sector_index < get_world_volume_in_sectors(); sector_index++)
    {
        SectorCoordinate sector_coordinate;
        sector_index_to_sector_coordinate(sector_index, sector_coordinate);

        m_sector_array[sector_index].init(sector_index);
    }

    return true;
}

void World::update(float dt)
{
    while (!InputSystem::input_action_queue.is_empty())
    {
        InputAction input_action = InputSystem::input_action_queue.pop();

        switch (input_action.input_action_type)
        {
        case INPUT_ACTION_PLACE:
        {
            HitResult hit_result = line_trace(input_action.place.origin, input_action.place.direction, 8.0f);

            if (hit_result.hit_cell && hit_result.previous_cell)
            {
                GridCoordinate grid_coordinate;
    
                indices_to_grid_coordinate(
                    hit_result.previous_cell->m_sector_index, 
                    hit_result.previous_cell->m_cell_index, 
                    grid_coordinate
                );

                set_block_kind(grid_coordinate, BLOCK_KIND_WOLF);
            }

            break;
        }
        case INPUT_ACTION_REMOVE:
        {
            HitResult hit_result = line_trace(input_action.remove.origin, input_action.remove.direction, 8.0f);
            
            if (hit_result.hit_cell)
            {
                GridCoordinate grid_coordinate;
    
                indices_to_grid_coordinate(
                    hit_result.hit_cell->m_sector_index, 
                    hit_result.hit_cell->m_cell_index, 
                    grid_coordinate
                );

                set_block_kind(grid_coordinate, BLOCK_KIND_NONE);
            }

            break;
        }
        default:
            break;
        }
    }
}

HitResult World::line_trace(vec3 origin, vec3 direction, float distance)
{
    HitResult hit_result = {
        .hit_cell = NULL,
        .previous_cell = NULL,
    };

    GridCoordinate grid_coordinate;

    world_position_to_grid_coordinate(origin, grid_coordinate);

    int step_x = (direction[0] > 0) ? 1 : (direction[0] < 0) ? -1 : 0;
    int step_y = (direction[1] > 0) ? 1 : (direction[1] < 0) ? -1 : 0;
    int step_z = (direction[2] > 0) ? 1 : (direction[2] < 0) ? -1 : 0;

    float t_delta_x = (direction[0] != 0) ? (1.0f / fabsf(direction[0])) : FLT_MAX;
    float t_delta_y = (direction[1] != 0) ? (1.0f / fabsf(direction[1])) : FLT_MAX;
    float t_delta_z = (direction[2] != 0) ? (1.0f / fabsf(direction[2])) : FLT_MAX;

    float next_voxel_boundary_x = grid_coordinate[0] + (step_x > 0 ? 1.0f : 0.0f);
    float next_voxel_boundary_y = grid_coordinate[1] + (step_y > 0 ? 1.0f : 0.0f);
    float next_voxel_boundary_z = grid_coordinate[2] + (step_z > 0 ? 1.0f : 0.0f);

    float t_max_x = (direction[0] != 0)
        ? (next_voxel_boundary_x - origin[0]) / direction[0]
        : FLT_MAX;

    float t_max_y = (direction[1] != 0)
        ? (next_voxel_boundary_y - origin[1]) / direction[1]
        : FLT_MAX;

    float t_max_z = (direction[2] != 0)
        ? (next_voxel_boundary_z - origin[2]) / direction[2]
        : FLT_MAX;

    int MAX_FRAMES = 128;

    for (int i = 0; i < MAX_FRAMES; ++i)
    {
        float current_t = std::min(std::min(t_max_x, t_max_y), t_max_z);

        if (current_t > distance)
        {
            break;
        }

        Cell* cell = get_cell(grid_coordinate);

        if (!cell)
        {
            return hit_result;
        }

        if (cell->m_block_kind != BLOCK_KIND_NONE)
        {
            hit_result.hit_cell = cell;
            return hit_result;
        }

        hit_result.previous_cell = cell;
        
        if (t_max_x <= t_max_y && t_max_x <= t_max_z)
        {
            grid_coordinate[0] += step_x;
            t_max_x += t_delta_x;
        }
        else if (t_max_y <= t_max_z)
        {
            grid_coordinate[1] += step_y;
            t_max_y += t_delta_y;
        }
        else
        {
            grid_coordinate[2] += step_z;
            t_max_z += t_delta_z;
        }

        if (!grid_coordinate_is_valid(grid_coordinate))
        {
            break;
        }
    }

    return hit_result;
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
                GridCoordinate grid_coordinate = {x,y,z};

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
                GridCoordinate grid_coordinate = {x, y, z};

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