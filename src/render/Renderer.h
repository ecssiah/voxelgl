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

    GLuint m_vao_id = 0;
    GLuint m_vbo_id = 0;
    GLuint m_ebo_id = 0;

    GLuint m_program_id = 0;
    GLuint m_texture_id = 0;

    mat4 m_view_projection_matrix;

    bool init();

    void update(World* world);
    void render(mat4 view_matrix, mat4 projection_matrix);

    void draw_mesh(GpuMesh* gpu_mesh);

    void build_sector_mesh(Sector* sector, SectorMesh* out_sector_mesh);
    void upload_mesh(SectorMesh* sector_mesh, GpuMesh* gpu_mesh);

};