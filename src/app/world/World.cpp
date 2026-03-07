#include "app/world/world.h"

#include <cstddef>
#include <cstdio>
#include <cfloat>
#include <stdlib.h>
#include <time.h>

#include <GLFW/glfw3.h>

#include "core/utils.h"
#include "platform/input.h"
#include "app/world/grid.h"
#include "app/action/action.h"

World* world_create()
{
    World* world = (World*)malloc(sizeof (World));

    return world;
}

bool world_init(World* world)
{
    srand((unsigned int)time(NULL));

    for (SectorIndex sector_index = 0; sector_index < get_world_volume_in_sectors(); sector_index++)
    {
        SectorCoordinate sector_coordinate;
        sector_index_to_sector_coordinate(sector_index, sector_coordinate);

        Sector* sector = &world->sector_array[sector_index];

        sector->sector_index = sector_index;
        sector->version = 1;

        for (CellIndex cell_index = 0; cell_index < get_sector_volume_in_cells(); cell_index++)
        {
            CellCoordinate cell_coordinate;
            cell_index_to_cell_coordinate(cell_index, cell_coordinate);

            Cell* cell = &sector->cell_array[cell_index];

            cell->cell_index = cell_index;
            cell->sector_index = sector_index;
            cell->cell_face_mask = 0;
            cell->block_kind = BLOCK_KIND_NONE;
        }
    }

    world_setup_demo_world(world);

    return true;
}

void world_update(World* world, Input* input, f32 dt)
{
    while (!action_queue_is_empty(&input->action_queue))
    {
        Action action = action_queue_pop(&input->action_queue);

        switch (action.action_type)
        {
        case ACTION_PLACE:
        {
            HitResult hit_result = world_line_trace(world, action.place.origin, action.place.direction, 8.0f);

            if (hit_result.hit_cell && hit_result.previous_cell)
            {
                GridCoordinate grid_coordinate;
    
                indices_to_grid_coordinate(
                    hit_result.previous_cell->sector_index, 
                    hit_result.previous_cell->cell_index, 
                    grid_coordinate
                );

                world_set_block_kind(world, grid_coordinate, BLOCK_KIND_WOLF);
            }

            break;
        }
        case ACTION_REMOVE:
        {
            HitResult hit_result = world_line_trace(world, action.remove.origin, action.remove.direction, 8.0f);
            
            if (hit_result.hit_cell)
            {
                GridCoordinate grid_coordinate;
    
                indices_to_grid_coordinate(
                    hit_result.hit_cell->sector_index, 
                    hit_result.hit_cell->cell_index, 
                    grid_coordinate
                );

                world_set_block_kind(world, grid_coordinate, BLOCK_KIND_NONE);
            }

            break;
        }
        default:
            break;
        }
    }
}

Cell* world_get_cell(World* world, GridCoordinate grid_coordinate)
{
    if (grid_coordinate_is_valid(grid_coordinate))
    {
        const SectorIndex sector_index = grid_coordinate_to_sector_index(grid_coordinate);
        const CellIndex cell_index = grid_coordinate_to_cell_index(grid_coordinate);

        Sector* sector = &world->sector_array[sector_index];

        return &sector->cell_array[cell_index];
    }
    else 
    {
        return NULL;
    }
}

void world_set_block_kind(World* world, GridCoordinate grid_coordinate, BlockKind block_kind)
{
    if (!grid_coordinate_is_valid(grid_coordinate))
    {
        printf("Invalid set position: (%d %d %d)\n", grid_coordinate[0], grid_coordinate[1], grid_coordinate[2]);

        return;
    }

    const SectorIndex sector_index = grid_coordinate_to_sector_index(grid_coordinate);
    const CellIndex cell_index = grid_coordinate_to_cell_index(grid_coordinate);

    Sector* sector = &world->sector_array[sector_index];
    Cell* cell = &sector->cell_array[cell_index];

    cell->block_kind = block_kind;

    w_update_face_exposure(world, cell, grid_coordinate);

    sector->version += 1;
}

void world_set_block_kind_wireframe(World* world, GridCoordinate grid_coordinate_min, GridCoordinate grid_coordinate_max, BlockKind block_kind)
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
                    world_set_block_kind(world, grid_coordinate, block_kind);
                }
            }
        }
    }
}

void world_set_block_kind_cube(World* world, GridCoordinate grid_coordinate_min, GridCoordinate grid_coordinate_max, BlockKind block_kind)
{
    for (int z = grid_coordinate_min[2]; z <= grid_coordinate_max[2]; ++z)
    {
        for (int y = grid_coordinate_min[1]; y <= grid_coordinate_max[1]; ++y)
        {
            for (int x = grid_coordinate_min[0]; x <= grid_coordinate_max[0]; ++x)
            {
                GridCoordinate grid_coordinate = {x, y, z};

                world_set_block_kind(world, grid_coordinate, block_kind);
            }
        }
    }
}

void w_update_face_exposure(World* world, Cell* cell, GridCoordinate grid_coordinate)
{
    w_update_cell_face_mask(world, cell, grid_coordinate);

    for (int cell_face_index = 0; cell_face_index < CELL_FACE_COUNT; ++cell_face_index)
    {
        GridCoordinate neighbor_grid_coordinate;
        glm_ivec3_add(grid_coordinate, CELL_FACE_OFFSET_ARRAY[cell_face_index], neighbor_grid_coordinate);

        Cell* neighbor_cell = world_get_cell(world, neighbor_grid_coordinate);

        if (neighbor_cell)
        {
            w_update_cell_face_mask(world, neighbor_cell, neighbor_grid_coordinate);

            const SectorIndex neighbor_sector_index = grid_coordinate_to_sector_index(neighbor_grid_coordinate);
            world->sector_array[neighbor_sector_index].version += 1;
        }
    }
}

void w_update_cell_face_mask(World* world, Cell* cell, GridCoordinate grid_coordinate)
{
    cell->cell_face_mask = 0;

    if (cell->block_kind == BLOCK_KIND_NONE)
    {
        return;
    }

    for (int cell_face_index = 0; cell_face_index < CELL_FACE_COUNT; ++cell_face_index)
    {
        GridCoordinate neighbor_grid_coordinate;
        glm_ivec3_add(grid_coordinate, CELL_FACE_OFFSET_ARRAY[cell_face_index], neighbor_grid_coordinate);

        Cell* neighbor_cell = world_get_cell(world, neighbor_grid_coordinate);

        if (neighbor_cell == NULL || neighbor_cell->block_kind == BLOCK_KIND_NONE)
        {
            cell->cell_face_mask |= CELL_FACE_BIT(cell_face_index);
        }
    }
}

HitResult world_line_trace(World* world, vec3 origin, vec3 direction, f32 distance)
{
    HitResult hit_result = {
        .hit_cell = NULL,
        .previous_cell = NULL,
    };

    GridCoordinate grid_coordinate;

    world_position_to_grid_coordinate(origin, grid_coordinate);

    Cell* cell = world_get_cell(world, grid_coordinate);

    if (!cell)
    {
        return hit_result;
    }

    if (cell->block_kind != BLOCK_KIND_NONE)
    {
        hit_result.hit_cell = cell;
        return hit_result;
    }

    hit_result.previous_cell = cell;

    int step_x = (direction[0] > 0) ? 1 : (direction[0] < 0) ? -1 : 0;
    int step_y = (direction[1] > 0) ? 1 : (direction[1] < 0) ? -1 : 0;
    int step_z = (direction[2] > 0) ? 1 : (direction[2] < 0) ? -1 : 0;

    f32 t_delta_x = (direction[0] != 0) ? (1.0f / fabsf(direction[0])) : FLT_MAX;
    f32 t_delta_y = (direction[1] != 0) ? (1.0f / fabsf(direction[1])) : FLT_MAX;
    f32 t_delta_z = (direction[2] != 0) ? (1.0f / fabsf(direction[2])) : FLT_MAX;

    f32 next_voxel_boundary_x = grid_coordinate[0] + (step_x > 0 ? CELL_RADIUS : -CELL_RADIUS);
    f32 next_voxel_boundary_y = grid_coordinate[1] + (step_y > 0 ? CELL_RADIUS : -CELL_RADIUS);
    f32 next_voxel_boundary_z = grid_coordinate[2] + (step_z > 0 ? CELL_RADIUS : -CELL_RADIUS);

    f32 t_max_x = (direction[0] != 0)
        ? (next_voxel_boundary_x - origin[0]) / direction[0]
        : FLT_MAX;

    f32 t_max_y = (direction[1] != 0)
        ? (next_voxel_boundary_y - origin[1]) / direction[1]
        : FLT_MAX;

    f32 t_max_z = (direction[2] != 0)
        ? (next_voxel_boundary_z - origin[2]) / direction[2]
        : FLT_MAX;

    int MAX_FRAMES = 128;

    glm_vec3_print(origin, stdout);

    for (int i = 0; i < MAX_FRAMES; ++i)
    {
        glm_ivec3_print(grid_coordinate, stdout);

        f32 current_t = MIN3(t_max_x, t_max_y, t_max_z);

        if (current_t > distance)
        {
            break;
        }

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

        cell = world_get_cell(world, grid_coordinate);

        if (!cell)
        {
            return hit_result;
        }

        if (cell->block_kind != BLOCK_KIND_NONE)
        {
            hit_result.hit_cell = cell;
            return hit_result;
        }

        hit_result.previous_cell = cell;

        if (!grid_coordinate_is_valid(grid_coordinate))
        {
            break;
        }
    }

    return hit_result;
}

void world_setup_demo_world(World* world)
{
    const int OFFSET = 8;
    const int RADIUS = 1;

    world_set_block_kind(world, ivec3{+1, +0, +0}, BLOCK_KIND_EAGLE);
    world_set_block_kind(world, ivec3{-1, +0, +0}, BLOCK_KIND_LION);
    world_set_block_kind(world, ivec3{+0, +1, +0}, BLOCK_KIND_WOLF);
    world_set_block_kind(world, ivec3{+0, -1, +0}, BLOCK_KIND_HORSE);

    world_set_block_kind_cube(world, 
        ivec3{-8, -4, -8}, 
        ivec3{+8, -4, +8}, 
        BLOCK_KIND_STONE
    );

    world_set_block_kind_cube(world, 
        ivec3{+OFFSET - RADIUS, -RADIUS, -RADIUS}, 
        ivec3{+OFFSET + RADIUS, +RADIUS, +RADIUS}, 
        BLOCK_KIND_EAGLE
    );

    world_set_block_kind_cube(world, 
        ivec3{-OFFSET - RADIUS, -RADIUS, -RADIUS}, 
        ivec3{-OFFSET + RADIUS, +RADIUS, +RADIUS}, 
        BLOCK_KIND_LION
    );

    world_set_block_kind_cube(world, 
        ivec3{-RADIUS, -RADIUS, OFFSET - RADIUS}, 
        ivec3{+RADIUS, +RADIUS, OFFSET + RADIUS}, 
        BLOCK_KIND_HORSE
    );

    world_set_block_kind_cube(world, 
        ivec3{-RADIUS, -RADIUS, -OFFSET - RADIUS}, 
        ivec3{+RADIUS, +RADIUS, -OFFSET + RADIUS}, 
        BLOCK_KIND_WOLF
    );
}