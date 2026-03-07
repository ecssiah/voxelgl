#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "app/world/grid.h"
#include "app/world/world.h"
#include "render/gpu_mesh.h"
#include "render/sector_mesh.h"

struct Renderer
{
    SectorMesh sector_mesh_cache[get_world_volume_in_sectors()];
    GpuMesh gpu_mesh_cache[get_world_volume_in_sectors()];

    GLuint program_id = 0;
    GLuint texture_array_id = 0;

    GLuint mvp_uniform_location = 0;
    GLuint texture_sampler_location = 0;

    mat4 view_projection_matrix = GLM_MAT4_IDENTITY_INIT;
};

Renderer* renderer_create();
bool renderer_init(Renderer* renderer);
void renderer_update(Renderer* renderer, World* world);
void renderer_render(Renderer* renderer, mat4 view_matrix, mat4 projection_matrix);

void r_draw_mesh(Renderer* renderer, GpuMesh* gpu_mesh);

void r_load_texture_array(Renderer* renderer, const char* directory);

void r_build_sector_mesh(Renderer* renderer, Sector* sector, SectorMesh* out_sector_mesh);
void r_upload_mesh(Renderer* renderer, SectorMesh* sector_mesh, GpuMesh* gpu_mesh);
void r_emit_face(Renderer* renderer, SectorMesh* sector_mesh, vec3 sector_position, CellFace cell_face, BlockKind cell_block_kind);
