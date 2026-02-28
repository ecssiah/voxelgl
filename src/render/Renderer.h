#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "app/world/grid.h"
#include "app/world/world.h"
#include "render/gpu_mesh.h"
#include "render/sector_mesh.h"

struct Renderer
{
    SectorMesh m_sector_mesh_cache[WORLD_VOLUME_IN_SECTORS];
    GpuMesh m_gpu_mesh_cache[WORLD_VOLUME_IN_SECTORS];

    GLuint m_program_id = 0;
    GLuint m_texture_array_id = 0;

    GLuint m_mvp_uniform_location = 0;
    GLuint m_texture_sampler_location = 0;

    mat4 m_view_projection_matrix;

    bool init();

    void update(World* world);
    void render(mat4 view_matrix, mat4 projection_matrix);

    void draw_mesh(GpuMesh* gpu_mesh);

    void load_texture_array(const char* directory);

    void build_sector_mesh(Sector* sector, SectorMesh* out_sector_mesh);
    void upload_mesh(SectorMesh* sector_mesh, GpuMesh* gpu_mesh);
    void emit_face(SectorMesh* sector_mesh, vec3 world_position, CellFace cell_face, BlockKind cell_block_kind);

};