#include "core/types.h"

struct VoxelVertex
{
    f32 position[3];
    f32 normal[3];
    f32 uv[2];
    u32 texture_index;
};