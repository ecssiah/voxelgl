#pragma once

#include <cglm/cglm.h>

#include "core/types.h"
#include "renderer/voxel_vertex.h"

struct SectorMesh
{
    u32 version;

    VoxelVertex* vertex_data_array;
    u32 vertex_data_count;
    u32 vertex_data_capacity;

    u32* index_array;
    u32 index_count;
    u32 index_capacity;
};