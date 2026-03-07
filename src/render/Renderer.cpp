#include "render/renderer.h"

#include <string>
#include <cassert>
#include <cstdio>
#include <stb_image.h>

#include "app/world/world.h"
#include "app/world/grid.h"
#include "render/block_render_data.h"
#include "render/sector_mesh.h"
#include "utils/shader_utils.h"

Renderer* renderer_create()
{
    Renderer* renderer = (Renderer*)malloc(sizeof (Renderer));

    return renderer;
}

bool renderer_init(Renderer* renderer) 
{
    std::string vert_shader_src = shader_load("assets/shaders/voxel.vert");
    std::string frag_shader_src = shader_load("assets/shaders/voxel.frag");

    GLuint vert_shader_id = shader_compile(GL_VERTEX_SHADER, vert_shader_src.c_str());
    GLuint frag_shader_id = shader_compile(GL_FRAGMENT_SHADER, frag_shader_src.c_str());

    renderer->program_id = glCreateProgram();

    glAttachShader(renderer->program_id, vert_shader_id);
    glAttachShader(renderer->program_id, frag_shader_id);

    glLinkProgram(renderer->program_id);

    int linked = 0;
    glGetProgramiv(renderer->program_id, GL_LINK_STATUS, &linked);

    if (!linked) 
    {
        char log[1024];
        glGetProgramInfoLog(renderer->program_id, sizeof(log), nullptr, log);

        fprintf(stderr, "[PROGRAM LINK ERROR]\n%s\n", log);
    }

    stbi_set_flip_vertically_on_load(true);

    renderer->mvp_uniform_location = glGetUniformLocation(renderer->program_id, "u_mvp_matrix");
    renderer->texture_sampler_location = glGetUniformLocation(renderer->program_id, "u_texture_sampler");

    if (renderer->texture_sampler_location != -1) 
    {
        glUseProgram(renderer->program_id);
        glUniform1i(renderer->texture_sampler_location, 0);
    }

    r_load_texture_array(renderer, "assets/textures");

    glDeleteShader(vert_shader_id);
    glDeleteShader(frag_shader_id);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void r_load_texture_array(Renderer* renderer, const char* directory)
{
    glGenTextures(1, &renderer->texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderer->texture_array_id);
    
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
            0, 
            0,
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

void renderer_update(Renderer* renderer, World* world)
{
    for (SectorIndex sector_index = 0; sector_index < get_world_volume_in_sectors(); sector_index++)
    {
        Sector* sector = &world->sector_array[sector_index];

        SectorMesh* sector_mesh = &renderer->sector_mesh_cache[sector_index];
        GpuMesh* gpu_mesh = &renderer->gpu_mesh_cache[sector_index];

        r_build_sector_mesh(renderer, sector, sector_mesh);
        r_upload_mesh(renderer, sector_mesh, gpu_mesh);
    }
}

void r_build_sector_mesh(Renderer* renderer, Sector* sector, SectorMesh* out_sector_mesh)
{
    if (sector->version <= out_sector_mesh->version) 
    {
        return;
    }

    SectorCoordinate sector_coordinate;
    sector_index_to_sector_coordinate(sector->sector_index, sector_coordinate);

    GridCoordinate sector_grid_coordinate;
    sector_coordinate_to_grid_coordinate(sector_coordinate, sector_grid_coordinate);

    vec3 sector_world_position;
    grid_coordinate_to_world_position(sector_grid_coordinate, sector_world_position);

    glm_vec3_copy(sector_world_position, out_sector_mesh->sector_world_position);

    out_sector_mesh->vertex_vec.clear();
    out_sector_mesh->index_vec.clear();

    for (CellIndex cell_index = 0; cell_index < get_sector_volume_in_cells(); ++cell_index)
    {
        Cell* cell = &sector->cell_array[cell_index];

        if (cell->block_kind == BLOCK_KIND_NONE)
        {
            continue;
        }

        if (cell->cell_face_mask == 0)
        {
            continue;
        }

        CellCoordinate cell_coordinate;
        cell_index_to_cell_coordinate(cell_index, cell_coordinate);

        vec3 cell_sector_position;
        cell_sector_position[0] = (f32)cell_coordinate[0];
        cell_sector_position[1] = (f32)cell_coordinate[1];
        cell_sector_position[2] = (f32)cell_coordinate[2];

        for (int cell_face = 0; cell_face < CELL_FACE_COUNT; ++cell_face)
        {
            if (cell->cell_face_mask & CELL_FACE_BIT(cell_face))
            {
                r_emit_face(
                    renderer,
                    out_sector_mesh, 
                    cell_sector_position, 
                    (CellFace)cell_face, 
                    cell->block_kind
                );
            }
        }
    }

    out_sector_mesh->version = sector->version;
}

void r_emit_face(Renderer* renderer, SectorMesh* sector_mesh, vec3 sector_position, CellFace cell_face, BlockKind block_kind)
{
    uint32_t base_index = sector_mesh->vertex_vec.size();

    for (int vertex_index = 0; vertex_index < VERTICES_PER_FACE; ++vertex_index)
    {
        VertexData vertex_data = {};

        glm_vec3_add(
            sector_position,
            FACE_VERTEX_ARRAY[cell_face][vertex_index],
            vertex_data.position
        );

        glm_vec3_copy(FACE_NORMAL_ARRAY[cell_face], vertex_data.normal);

        glm_vec2_copy(FACE_UV_ARRAY[cell_face][vertex_index], vertex_data.uv);

        vertex_data.texture_index = (u8)block_kind;

        sector_mesh->vertex_vec.push_back(vertex_data);
    }

    sector_mesh->index_vec.push_back(base_index + 0);
    sector_mesh->index_vec.push_back(base_index + 1);
    sector_mesh->index_vec.push_back(base_index + 2);

    sector_mesh->index_vec.push_back(base_index + 2);
    sector_mesh->index_vec.push_back(base_index + 3);
    sector_mesh->index_vec.push_back(base_index + 0);
}

void r_upload_mesh(Renderer* renderer, SectorMesh* sector_mesh, GpuMesh* out_gpu_mesh)
{
    if (sector_mesh->version <= out_gpu_mesh->version)
    {
        return;
    }

    if (out_gpu_mesh->vao_id == 0)
    {
        glGenVertexArrays(1, &out_gpu_mesh->vao_id);

        glGenBuffers(1, &out_gpu_mesh->vbo_id);
        glGenBuffers(1, &out_gpu_mesh->ebo_id);
    }

    glBindVertexArray(out_gpu_mesh->vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, out_gpu_mesh->vbo_id);

    glBufferData(
        GL_ARRAY_BUFFER,
        sector_mesh->vertex_vec.size() * sizeof(VertexData),
        sector_mesh->vertex_vec.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_gpu_mesh->ebo_id);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sector_mesh->index_vec.size() * sizeof(uint32_t),
        sector_mesh->index_vec.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)offsetof(VertexData, position)
    );

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)offsetof(VertexData, normal)
    );

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        (void*)offsetof(VertexData, uv)
    );

    glVertexAttribPointer(
        3,
        1,
        GL_UNSIGNED_BYTE,
        GL_FALSE,
        sizeof(VertexData),
        (void*)offsetof(VertexData, texture_index)
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    glm_mat4_identity(out_gpu_mesh->model_matrix);
    glm_translate(out_gpu_mesh->model_matrix, sector_mesh->sector_world_position);

    out_gpu_mesh->index_count = sector_mesh->index_vec.size();

    out_gpu_mesh->version = sector_mesh->version;
}

void renderer_render(Renderer* renderer, mat4 view_matrix, mat4 projection_matrix) 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm_mat4_mul(projection_matrix, view_matrix, renderer->view_projection_matrix);

    glUseProgram(renderer->program_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderer->texture_array_id);

    for (SectorIndex sector_index = 0; sector_index < get_world_volume_in_sectors(); sector_index++)
    {
        GpuMesh* gpu_mesh = &renderer->gpu_mesh_cache[sector_index];

        if (gpu_mesh->vao_id != 0)
        {
            r_draw_mesh(renderer, gpu_mesh);
        }
    }
}

void r_draw_mesh(Renderer* renderer, GpuMesh* gpu_mesh)
{
    mat4 model_view_projection_matrix;
    glm_mat4_mul(renderer->view_projection_matrix, gpu_mesh->model_matrix, model_view_projection_matrix);

    glUniformMatrix4fv(
        renderer->mvp_uniform_location,
        1,
        GL_FALSE,
        (f32*)model_view_projection_matrix
    );

    glBindVertexArray(gpu_mesh->vao_id);

    glDrawElements(
        GL_TRIANGLES,
        gpu_mesh->index_count,
        GL_UNSIGNED_INT,
        0
    );
}
