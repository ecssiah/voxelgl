#pragma once

#include "app/world/grid.h"

struct ActionQueue;
struct Input;
struct Camera;

enum CellFace
{
    CELL_FACE_POS_X,
    CELL_FACE_NEG_X,
    CELL_FACE_POS_Y,
    CELL_FACE_NEG_Y,
    CELL_FACE_POS_Z,
    CELL_FACE_NEG_Z,
    CELL_FACE_COUNT,
};

#define CELL_FACE_BIT(cell_face) (1u << (cell_face))

static ivec3 CELL_FACE_OFFSET_ARRAY[CELL_FACE_COUNT] = {
    {+1, +0, +0},
    {-1, +0, +0},
    {+0, +1, +0},
    {+0, -1, +0},
    {+0, +0, +1},
    {+0, +0, -1},
};

enum BlockKind
{
    BLOCK_KIND_NONE,
    BLOCK_KIND_STONE,
    BLOCK_KIND_EAGLE,
    BLOCK_KIND_LION,
    BLOCK_KIND_WOLF,
    BLOCK_KIND_HORSE,
    BLOCK_KIND_COUNT,
};

static const char* BLOCK_KIND_NAME_ARRAY[BLOCK_KIND_COUNT] =
{
    "None",
    "Stone",
    "Eagle",
    "Lion",
    "Wolf",
    "Horse",
};

struct Cell
{
    CellIndex cell_index;
    SectorIndex sector_index;
    
    BlockKind block_kind;
    u8 cell_face_mask;
};

struct Sector
{
    u32 version;
    SectorIndex sector_index;
    
    Cell cell_array[get_sector_volume_in_cells()];
};

struct World
{
    Sector sector_array[get_world_volume_in_sectors()];
};

struct HitResult
{
    Cell* previous_cell;
    Cell* hit_cell;
};

World* world_create();
bool world_init(World* world);
void world_update(World* world, Input* input, Camera* camera, f32 dt);

Cell* world_get_cell(World* world, GridCoordinate grid_coordinate);

void world_set_block_kind(World* world, GridCoordinate grid_coordinate, BlockKind block_kind);
void world_set_block_kind_cube(World* world, GridCoordinate grid_coordinate_min, GridCoordinate grid_coordinate_max, BlockKind block_kind);
void world_set_block_kind_wireframe(World* world, GridCoordinate grid_coordinate_min, GridCoordinate grid_coordinate_max, BlockKind block_kind);

HitResult world_line_trace(World* world, vec3 origin, vec3 direction, f32 distance);

void world_setup_demo_world(World* world);

void w_update_face_exposure(World* world, Cell* cell, GridCoordinate grid_coordinate);
void w_update_cell_face_mask(World* world, Cell* cell, GridCoordinate grid_coordinate);

void w_sector_init(Sector* sector, SectorIndex sector_index);
void w_cell_init(Cell* cell, SectorIndex sector_index, CellIndex cell_index);