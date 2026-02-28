#pragma once

#include "sector.h"

struct World
{
    Sector m_sector_array[WORLD_VOLUME_IN_SECTORS];

    bool init();

    Cell* get_cell(GridCoordinate grid_coordinate);
    
    void set_block_kind(GridCoordinate grid_coordinate, BlockKind block_kind);
    void set_block_kind_cube(GridCoordinate grid_coordinate_min, GridCoordinate grid_coordinate_max, BlockKind block_kind);
    void set_block_kind_wireframe(GridCoordinate grid_coordinate_min, GridCoordinate grid_coordinate_max, BlockKind block_kind);

    void update_face_exposure(Cell* cell, GridCoordinate grid_coordinate);
    void update_cell_face_mask(Cell* cell, GridCoordinate grid_coordinate);
};