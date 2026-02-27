#include "render/renderer.h"
#include "render/voxel_data.h"
#include "app/world/grid.h"
#include "utils/shader_utils.h"
#include "utils/stb_utils.h"
#include <cstdio>
#include <iostream>
#include <string>

bool Renderer::init() 
{
    glGenVertexArrays(1, &m_vao_id);

    glGenBuffers(1, &m_vbo_id);
    glGenBuffers(1, &m_ebo_id);

    glBindVertexArray(m_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);

    glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(VOXEL_VERTEX_ARRAY), 
        VOXEL_VERTEX_ARRAY, 
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        sizeof(VOXEL_INDEX_ARRAY),
        VOXEL_INDEX_ARRAY, 
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertexData),
        (void*)offsetof(VoxelVertexData, position_array)
    );

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertexData),
        (void*)offsetof(VoxelVertexData, normal_array)
    );

    glVertexAttribPointer(
        2, 
        2, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(VoxelVertexData),
        (void*)offsetof(VoxelVertexData, uv_array)
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    const std::string vert_src = 
        shader_utils::load_text_file("assets/shaders/voxel.vert");

    const std::string frag_src = 
        shader_utils::load_text_file("assets/shaders/voxel.frag");

    if (vert_src.empty() || frag_src.empty()) 
    {
        return false;
    }

    const GLuint shader_vert = 
        shader_utils::compile_shader(GL_VERTEX_SHADER,vert_src.c_str());

    const GLuint shader_frag =
        shader_utils::compile_shader(GL_FRAGMENT_SHADER,frag_src.c_str());

    m_program_id = glCreateProgram();

    glAttachShader(m_program_id, shader_vert);
    glAttachShader(m_program_id, shader_frag);

    glLinkProgram(m_program_id);

    int linked = 0;
    glGetProgramiv(m_program_id, GL_LINK_STATUS, &linked);

    if (!linked) 
    {
        char log[1024];
        glGetProgramInfoLog(m_program_id, sizeof(log), nullptr, log);

        fprintf(stderr, "[PROGRAM LINK ERROR]\n%s\n", log);
    }

    glDeleteShader(shader_vert);
    glDeleteShader(shader_frag);

    m_texture_id = stb_utils::load_texture_2d("assets/textures/lion.png");

    if (m_texture_id == 0) 
    {
        std::cerr << "Texture failed to load\n";

        return false;
    }

    glEnable(GL_DEPTH_TEST);

    return true;
}

void Renderer::update(World* world)
{
    for (SectorIndex sector_index = 0; sector_index < WORLD_SIZE_IN_SECTORS; sector_index++)
    {
        Sector* sector = &world->m_sector_array[sector_index];

        SectorMesh* sector_mesh = &m_sector_mesh_cache[sector_index];
        GpuMesh* gpu_mesh = &m_gpu_mesh_cache[sector_index];

        build_sector_mesh(sector, sector_mesh);
        upload_mesh(sector_mesh, gpu_mesh);
    }
}

void Renderer::build_sector_mesh(Sector* sector, SectorMesh* out_sector_mesh)
{
    if (sector->m_version <= out_sector_mesh->m_version) 
    {
        return;
    }

    SectorCoordinate sector_coordinate;
    sector_index_to_sector_coordinate(sector->m_sector_index, sector_coordinate);

    ivec3 sector_grid_position;
    sector_coordinate_to_grid_position(sector_coordinate, sector_grid_position);

    vec3 sector_world_position;
    grid_position_to_world_position(sector_grid_position, sector_world_position);

    glm_vec3_copy(sector_world_position, out_sector_mesh->m_sector_world_position);

    

    out_sector_mesh->m_version = sector->m_version;
}

void Renderer::upload_mesh(SectorMesh* sector_mesh, GpuMesh* out_gpu_mesh)
{
    if (sector_mesh->m_version <= out_gpu_mesh->m_version)
    {
        return;
    }

    if (out_gpu_mesh->m_vao_id == 0)
    {
        glGenVertexArrays(1, &out_gpu_mesh->m_vao_id);

        glGenBuffers(1, &out_gpu_mesh->m_vbo_id);
        glGenBuffers(1, &out_gpu_mesh->m_ebo_id);
    }

    glBindVertexArray(out_gpu_mesh->m_vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, out_gpu_mesh->m_vbo_id);

    glBufferData(
        GL_ARRAY_BUFFER,
        sector_mesh->m_vertex_vec.size() * sizeof(VertexData),
        sector_mesh->m_vertex_vec.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_gpu_mesh->m_ebo_id);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sector_mesh->m_index_vec.size() * sizeof(uint32_t),
        sector_mesh->m_index_vec.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)offsetof(VertexData, m_position)
    );

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glm_mat4_identity(out_gpu_mesh->m_model_matrix);
    glm_translate(out_gpu_mesh->m_model_matrix, sector_mesh->m_sector_world_position);

    out_gpu_mesh->m_index_count = sector_mesh->m_index_vec.size();
    out_gpu_mesh->m_version = sector_mesh->m_version;
}

void Renderer::render(mat4 view_matrix, mat4 projection_matrix) 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm_mat4_mul(projection_matrix, view_matrix, m_view_projection_matrix);

    glUseProgram(m_program_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    const GLint texture_sampler_id = glGetUniformLocation(m_program_id, "u_texture_sampler");

    if (texture_sampler_id != -1) 
    {
        glUniform1i(texture_sampler_id, 0);
    }

    for (SectorIndex sector_index = 0; sector_index < WORLD_VOLUME_IN_SECTORS; sector_index++)
    {
        GpuMesh* gpu_mesh = &m_gpu_mesh_cache[sector_index];

        if (gpu_mesh) 
        {
            mat4 model_view_projection_matrix;
            glm_mat4_mul(m_view_projection_matrix, gpu_mesh->m_model_matrix, model_view_projection_matrix);

            glUniformMatrix4fv(
                glGetUniformLocation(m_program_id, "u_mvp_matrix"),
                1,
                GL_FALSE,
                (float*)model_view_projection_matrix
            );

            draw_mesh(gpu_mesh);
        }
    }
}

void Renderer::draw_mesh(GpuMesh* gpu_mesh)
{
    glBindVertexArray(gpu_mesh->m_vao_id);

    glDrawElements(
        GL_TRIANGLES,
        gpu_mesh->m_index_count,
        GL_UNSIGNED_INT,
        0
    );
}
