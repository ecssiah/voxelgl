#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "GpuMesh.h"
#include "app/world/World.h"
#include "render/SectorMesh.h"

class Renderer
{

public:

    bool init();

    void update(World* world);
    void render(mat4 view_matrix, mat4 projection_matrix);

    void draw_mesh(GpuMesh* gpu_mesh);

private:

    void calculate_mvp(mat4 view_matrix, mat4 projection_matrix, mat4 out_mvp_matrix);

    void build_sector_mesh(Sector* sector, SectorMesh* out_sector_mesh);
    void upload_mesh(SectorMesh* sector_mesh, GpuMesh* gpu_mesh);

    GLuint m_vao_id = 0;
    GLuint m_vbo_id = 0;
    GLuint m_ebo_id = 0;

    GLuint m_program_id = 0;
    GLuint m_texture_id = 0;

    mat4 m_model_matrix;

};