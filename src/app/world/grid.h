#pragma once

#include <cmath>

#include <cglm/cglm.h>

#include "core/types.h"

typedef u32 SectorIndex;
typedef ivec3 SectorCoordinate;

typedef u32 CellIndex;
typedef ivec3 CellCoordinate;

typedef ivec3 GridCoordinate;

// Dimensions

static constexpr i32 WORLD_RADIUS_IN_SECTORS = 2;
static constexpr i32 SECTOR_RADIUS_IN_CELLS = 2;
static constexpr f32 CELL_RADIUS = 0.5f;

static constexpr i32 get_world_size_in_sectors()
{
    return 2 * WORLD_RADIUS_IN_SECTORS + 1;
}

static constexpr i32 get_world_area_in_sectors()
{
    i32 world_size_in_sectors = get_world_size_in_sectors();

    return world_size_in_sectors * world_size_in_sectors;
}

static constexpr i32 get_world_volume_in_sectors()
{
    i32 world_size_in_sectors = get_world_size_in_sectors();

    return world_size_in_sectors * world_size_in_sectors * world_size_in_sectors;
}

static constexpr i32 get_sector_size_in_cells()
{
    return 2 * SECTOR_RADIUS_IN_CELLS + 1;
}

static constexpr i32 get_sector_area_in_cells()
{
    i32 sector_size_in_cells = get_sector_size_in_cells();

    return sector_size_in_cells * sector_size_in_cells;
}

static constexpr i32 get_sector_volume_in_cells()
{
    i32 sector_size_in_cells = get_sector_size_in_cells();

    return sector_size_in_cells * sector_size_in_cells * sector_size_in_cells;
}

static constexpr i32 get_world_radius_in_cells()
{
    return WORLD_RADIUS_IN_SECTORS * get_sector_size_in_cells() + SECTOR_RADIUS_IN_CELLS;
}

static constexpr f32 get_cell_size()
{
    return 2 * CELL_RADIUS;
}

static constexpr f32 get_cell_area()
{
    f32 cell_size = get_cell_size();

    return cell_size * cell_size;
}

static constexpr f32 get_cell_volume()
{
    f32 cell_size = get_cell_size();

    return cell_size * cell_size * cell_size;
}

// Indexing

static const bool sector_index_is_valid(SectorIndex sector_index)
{
    return sector_index < get_world_volume_in_sectors();
}

static const bool cell_index_is_valid(CellIndex cell_index)
{
    return cell_index < get_sector_volume_in_cells();
}

static const bool sector_coordinate_is_valid(SectorCoordinate sector_coordinate)
{
    const bool in_x_range = sector_coordinate[0] >= -WORLD_RADIUS_IN_SECTORS && sector_coordinate[0] <= WORLD_RADIUS_IN_SECTORS;
    const bool in_y_range = sector_coordinate[1] >= -WORLD_RADIUS_IN_SECTORS && sector_coordinate[1] <= WORLD_RADIUS_IN_SECTORS;
    const bool in_z_range = sector_coordinate[2] >= -WORLD_RADIUS_IN_SECTORS && sector_coordinate[2] <= WORLD_RADIUS_IN_SECTORS;

    return in_x_range && in_y_range && in_z_range;
}

static const bool cell_coordinate_is_valid(CellCoordinate cell_coordinate)
{
    const bool in_x_range = cell_coordinate[0] >= -SECTOR_RADIUS_IN_CELLS && cell_coordinate[0] <= SECTOR_RADIUS_IN_CELLS;
    const bool in_y_range = cell_coordinate[1] >= -SECTOR_RADIUS_IN_CELLS && cell_coordinate[1] <= SECTOR_RADIUS_IN_CELLS;
    const bool in_z_range = cell_coordinate[2] >= -SECTOR_RADIUS_IN_CELLS && cell_coordinate[2] <= SECTOR_RADIUS_IN_CELLS;

    return in_x_range && in_y_range && in_z_range;
}

static const bool grid_coordinate_is_valid(GridCoordinate grid_coordinate)
{
    i32 world_radius_in_cells = get_world_radius_in_cells();

    const bool in_x_range = grid_coordinate[0] >= -world_radius_in_cells && grid_coordinate[0] <= world_radius_in_cells;
    const bool in_y_range = grid_coordinate[1] >= -world_radius_in_cells && grid_coordinate[1] <= world_radius_in_cells;
    const bool in_z_range = grid_coordinate[2] >= -world_radius_in_cells && grid_coordinate[2] <= world_radius_in_cells;

    return in_x_range && in_y_range && in_z_range;
}

static const void grid_coordinate_to_world_position(GridCoordinate grid_coordinate, vec3 out_world_position)
{
    f32 cell_size = get_cell_size();

    out_world_position[0] = (f32)grid_coordinate[0] * cell_size;
    out_world_position[1] = (f32)grid_coordinate[1] * cell_size;
    out_world_position[2] = (f32)grid_coordinate[2] * cell_size;
}

static const void sector_index_to_sector_coordinate(SectorIndex sector_index, SectorCoordinate out_sector_coordinate)
{
    i32 world_size_in_sectors = get_world_size_in_sectors();
    i32 world_area_in_sectors = get_world_area_in_sectors();

    out_sector_coordinate[0] = sector_index % world_size_in_sectors;
    out_sector_coordinate[1] = (sector_index / world_size_in_sectors) % world_size_in_sectors;
    out_sector_coordinate[2] = sector_index / world_area_in_sectors;

    glm_ivec3_subs(out_sector_coordinate, WORLD_RADIUS_IN_SECTORS, out_sector_coordinate);
}

static const SectorIndex sector_coordinate_to_sector_index(SectorCoordinate sector_coordinate)
{
    i32 world_size_in_sectors = get_world_size_in_sectors();
    i32 world_area_in_sectors = get_world_area_in_sectors();

    SectorCoordinate sector_coordinate_indexable;
    glm_ivec3_adds(sector_coordinate, WORLD_RADIUS_IN_SECTORS, sector_coordinate_indexable);

    SectorIndex out_sector_index = 
        sector_coordinate_indexable[0] +
        sector_coordinate_indexable[1] * world_size_in_sectors +
        sector_coordinate_indexable[2] * world_area_in_sectors;

    return out_sector_index;
}

static const void sector_coordinate_to_grid_coordinate(SectorCoordinate sector_coordinate, GridCoordinate out_grid_coordinate)
{
    glm_ivec3_scale(sector_coordinate, get_sector_size_in_cells(), out_grid_coordinate);
}

static const void cell_index_to_cell_coordinate(CellIndex cell_index, CellCoordinate out_cell_coordinate)
{
    i32 sector_size_in_cells = get_sector_size_in_cells();
    i32 sector_area_in_cells = get_sector_area_in_cells();

    out_cell_coordinate[0] = cell_index % sector_size_in_cells;
    out_cell_coordinate[1] = (cell_index / sector_size_in_cells) % sector_size_in_cells;
    out_cell_coordinate[2] = cell_index / sector_area_in_cells;

    glm_ivec3_subs(out_cell_coordinate, SECTOR_RADIUS_IN_CELLS, out_cell_coordinate);
}

static const CellIndex cell_coordinate_to_cell_index(CellCoordinate cell_coordinate)
{
    i32 sector_size_in_cells = get_sector_size_in_cells();
    i32 sector_area_in_cells = get_sector_area_in_cells();

    CellCoordinate cell_coordinate_indexable;
    glm_ivec3_adds(cell_coordinate, SECTOR_RADIUS_IN_CELLS, cell_coordinate_indexable);

    CellIndex out_cell_index = 
        cell_coordinate_indexable[0] +
        cell_coordinate_indexable[1] * sector_size_in_cells +
        cell_coordinate_indexable[2] * sector_area_in_cells;

    return out_cell_index;
}

static const void grid_coordinate_to_sector_coordinate(GridCoordinate grid_coordinate, SectorCoordinate out_sector_coordinate)
{
    i32 world_radius_in_cells = get_world_radius_in_cells();
    i32 sector_size_in_cells = get_sector_size_in_cells();

    GridCoordinate grid_coordinate_indexable;
    glm_ivec3_adds(grid_coordinate, world_radius_in_cells, grid_coordinate_indexable);

    SectorCoordinate sector_coordinate_indexable;
    glm_ivec3_divs(grid_coordinate_indexable, sector_size_in_cells, sector_coordinate_indexable);

    glm_ivec3_subs(sector_coordinate_indexable, WORLD_RADIUS_IN_SECTORS, out_sector_coordinate);
}

static const void grid_coordinate_to_cell_coordinate(GridCoordinate grid_coordinate, CellCoordinate out_cell_coordinate)
{
    i32 world_radius_in_cells = get_world_radius_in_cells();
    i32 sector_size_in_cells = get_sector_size_in_cells();

    GridCoordinate grid_coordinate_indexable;
    glm_ivec3_adds(grid_coordinate, world_radius_in_cells, grid_coordinate_indexable);

    CellCoordinate cell_coordinate_indexable;
    cell_coordinate_indexable[0] = grid_coordinate_indexable[0] % sector_size_in_cells;
    cell_coordinate_indexable[1] = grid_coordinate_indexable[1] % sector_size_in_cells;
    cell_coordinate_indexable[2] = grid_coordinate_indexable[2] % sector_size_in_cells;

    glm_ivec3_subs(cell_coordinate_indexable, SECTOR_RADIUS_IN_CELLS, out_cell_coordinate);
}

static const SectorIndex grid_coordinate_to_sector_index(GridCoordinate grid_coordinate)
{
    SectorCoordinate sector_coordinate;
    grid_coordinate_to_sector_coordinate(grid_coordinate, sector_coordinate);

    return sector_coordinate_to_sector_index(sector_coordinate);
}

static const CellIndex grid_coordinate_to_cell_index(GridCoordinate grid_coordinate)
{
    CellCoordinate cell_coordinate;
    grid_coordinate_to_cell_coordinate(grid_coordinate, cell_coordinate);

    return cell_coordinate_to_cell_index(cell_coordinate);
}

static const void world_position_to_grid_coordinate(vec3 world_position, GridCoordinate out_grid_coordinate)
{
    out_grid_coordinate[0] = (i32)floorf(world_position[0] + CELL_RADIUS);
    out_grid_coordinate[1] = (i32)floorf(world_position[1] + CELL_RADIUS);
    out_grid_coordinate[2] = (i32)floorf(world_position[2] + CELL_RADIUS);
}

static const void indices_to_grid_coordinate(SectorIndex sector_index, CellIndex cell_index, GridCoordinate out_grid_coordinate)
{
    i32 sector_size_in_cells = get_sector_size_in_cells();

    SectorCoordinate sector_coordinate;
    CellCoordinate cell_coordinate;

    sector_index_to_sector_coordinate(sector_index, sector_coordinate);
    cell_index_to_cell_coordinate(cell_index, cell_coordinate);

    glm_ivec3_scale(sector_coordinate, sector_size_in_cells, out_grid_coordinate);
    glm_ivec3_add(out_grid_coordinate, cell_coordinate, out_grid_coordinate);
}