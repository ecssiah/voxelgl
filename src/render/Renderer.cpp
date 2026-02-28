#include "render/renderer.h"
#include "app/world/cell.h"
#include "render/block_render_data.h"
#include "render/sector_mesh.h"
#include "app/world/grid.h"
#include "utils/shader_utils.h"
#include "utils/stb_utils.h"
#include <assert.h>
#include <cassert>
#include <cstdio>
#include <string>

bool Renderer::init() 
{
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

    stbi_set_flip_vertically_on_load(true);

    m_mvp_uniform_location = glGetUniformLocation(m_program_id, "u_mvp_matrix");
    m_texture_sampler_location = glGetUniformLocation(m_program_id, "u_texture_sampler");

    if (m_texture_sampler_location != -1) 
    {
        glUseProgram(m_program_id);
        glUniform1i(m_texture_sampler_location, 0);
    }

    load_texture_array("assets/textures");

    glDeleteShader(shader_vert);
    glDeleteShader(shader_frag);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void Renderer::load_texture_array(const char* directory)
{
    glGenTextures(1, &m_texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array_id);
    
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA8,
        BLOCK_TEXTURE_SIZE,
        BLOCK_TEXTURE_SIZE,
        BLOCK_KIND_COUNT,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (int block_kind = 0; block_kind < BLOCK_KIND_COUNT; ++block_kind)
    {
        const char* filename = BLOCK_KIND_TEXTURE_FILENAME[block_kind];

        if (!filename) 
        {
            continue;
        }

        int width, height, channels;

        char full_path[512];

        snprintf(
            full_path,
            sizeof(full_path),
            "%s/%s",
            directory,
            filename
        );

        stbi_uc* pixel_data = 
            stbi_load(
                full_path,
                &width,
                &height,
                &channels,
                4
            );

        assert(width == BLOCK_TEXTURE_SIZE && height == BLOCK_TEXTURE_SIZE);
        assert(pixel_data != NULL);

        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0, 0,
            block_kind,
            width,
            height,
            1,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixel_data
        );

        stbi_image_free(pixel_data);
    }
}

void Renderer::update(World* world)
{
    for (SectorIndex sector_index = 0; sector_index < WORLD_VOLUME_IN_SECTORS; sector_index++)
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

    out_sector_mesh->m_vertex_vec.clear();
    out_sector_mesh->m_index_vec.clear();

    for (CellIndex cell_index = 0; cell_index < SECTOR_VOLUME_IN_CELLS; ++cell_index)
    {
        Cell* cell = &sector->m_cell_array[cell_index];

        if (cell->m_block_kind == BLOCK_KIND_NONE)
        {
            continue;
        }

        if (cell->m_cell_face_mask == 0)
        {
            continue;
        }

        ivec3 cell_grid_position;
        indices_to_grid_position(sector->m_sector_index, cell_index, cell_grid_position);

        vec3 cell_world_position;
        grid_position_to_world_position(cell_grid_position, cell_world_position);

        for (int cell_face = 0; cell_face < CELL_FACE_COUNT; ++cell_face)
        {
            if (cell->m_cell_face_mask & CELL_FACE_BIT(cell_face))
            {
                emit_face(
                    out_sector_mesh, 
                    cell_world_position, 
                    (CellFace)cell_face, 
                    cell->m_block_kind
                );
            }
        }
    }

    out_sector_mesh->m_version = sector->m_version;
}

void Renderer::emit_face(SectorMesh* sector_mesh, vec3 world_position, CellFace cell_face, BlockKind block_kind)
{
    uint32_t base_index = sector_mesh->m_vertex_vec.size();

    for (int vertex_index = 0; vertex_index < VERTICES_PER_FACE; ++vertex_index)
    {
        VertexData vertex_data = {};

        glm_vec3_add(
            world_position,
            FACE_VERTEX_ARRAY[cell_face][vertex_index],
            vertex_data.m_position
        );

        glm_vec3_copy(FACE_NORMAL_ARRAY[cell_face], vertex_data.m_normal);
        
        glm_vec2_copy(FACE_UV_ARRAY[cell_face][vertex_index], vertex_data.m_uv);

        vertex_data.m_texture_index = (uint8_t)block_kind;

        sector_mesh->m_vertex_vec.push_back(vertex_data);
    }

    sector_mesh->m_index_vec.push_back(base_index + 0);
    sector_mesh->m_index_vec.push_back(base_index + 1);
    sector_mesh->m_index_vec.push_back(base_index + 2);

    sector_mesh->m_index_vec.push_back(base_index + 2);
    sector_mesh->m_index_vec.push_back(base_index + 3);
    sector_mesh->m_index_vec.push_back(base_index + 0);
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

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)offsetof(VertexData, m_normal)
    );

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)offsetof(VertexData, m_uv)
    );

    glVertexAttribPointer(
        3,
        1,
        GL_UNSIGNED_BYTE,
        GL_FALSE,
        sizeof(VertexData),
        (void*)offsetof(VertexData, m_texture_index)
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

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
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array_id);

    for (SectorIndex sector_index = 0; sector_index < WORLD_VOLUME_IN_SECTORS; sector_index++)
    {
        GpuMesh* gpu_mesh = &m_gpu_mesh_cache[sector_index];

        if (gpu_mesh->m_vao_id != 0)
        {
            draw_mesh(gpu_mesh);
        }
    }
}

void Renderer::draw_mesh(GpuMesh* gpu_mesh)
{
    mat4 model_view_projection_matrix;
    glm_mat4_mul(m_view_projection_matrix, gpu_mesh->m_model_matrix, model_view_projection_matrix);

    glUniformMatrix4fv(
        m_mvp_uniform_location,
        1,
        GL_FALSE,
        (float*)model_view_projection_matrix
    );

    glBindVertexArray(gpu_mesh->m_vao_id);

    glDrawElements(
        GL_TRIANGLES,
        gpu_mesh->m_index_count,
        GL_UNSIGNED_INT,
        0
    );
}
