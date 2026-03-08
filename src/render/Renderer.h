#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "app/world/grid.h"
#include "app/world/world.h"
#include "render/gpu_mesh.h"
#include "render/sector_mesh.h"

struct Renderer
{
    GLuint program_id;
    GLuint texture_array_id;
    
    GLuint mvp_uniform_location;
    GLuint texture_sampler_location;
    
    mat4 view_projection_matrix;

    SectorMesh sector_mesh_cache[get_world_volume_in_sectors()];
    GpuMesh gpu_mesh_cache[get_world_volume_in_sectors()];
};

Renderer* renderer_create();
bool renderer_init(Renderer* renderer);
void renderer_update(Renderer* renderer, World* world);
void renderer_render(Renderer* renderer, mat4 view_matrix, mat4 projection_matrix);

void renderer_draw_mesh(Renderer* renderer, GpuMesh* gpu_mesh);

void renderer_load_texture_array(Renderer* renderer, const char* directory);

void renderer_build_sector_mesh(Renderer* renderer, Sector* sector, SectorMesh* out_sector_mesh);
void renderer_upload_gpu_mesh(Renderer* renderer, SectorMesh* sector_mesh, GpuMesh* gpu_mesh);
void renderer_emit_face(Renderer* renderer, SectorMesh* sector_mesh, vec3 sector_position, CellFace cell_face, BlockKind cell_block_kind);
