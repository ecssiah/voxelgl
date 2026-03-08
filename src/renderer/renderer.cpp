#include "renderer/renderer.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stb_image.h>

#include "app/camera.h"
#include "app/world/world.h"
#include "app/world/grid.h"
#include "platform/window.h"
#include "interface/interface.h"
#include "interface/interface_vertex.h"
#include "renderer/block_render_data.h"
#include "renderer/sector_mesh.h"
#include "renderer/gpu_mesh.h"
#include "utils/shader_utils.h"

static const u32 SECTOR_MESH_INITIAL_VERTEX_CAPACITY = 100000;

Renderer* renderer_create()
{
    u32 world_volume_in_sectors = get_world_volume_in_sectors();

    Renderer* renderer = (Renderer*)malloc(sizeof (Renderer));

    renderer->sector_mesh_cache_array = (SectorMesh*)malloc(sizeof (SectorMesh) * world_volume_in_sectors);
    renderer->sector_gpu_mesh_cache_array = (GpuMesh*)malloc(sizeof (GpuMesh) * world_volume_in_sectors);

    for (u32 sector_index = 0; sector_index < world_volume_in_sectors; ++sector_index)
    {
        SectorMesh* sector_mesh = &renderer->sector_mesh_cache_array[sector_index];

        sector_mesh->vertex_data_count = 0;
        sector_mesh->vertex_data_capacity = SECTOR_MESH_INITIAL_VERTEX_CAPACITY;

        sector_mesh->index_count = 0;
        sector_mesh->index_capacity = SECTOR_MESH_INITIAL_VERTEX_CAPACITY * 6 / 4;

        sector_mesh->vertex_data_array = (VoxelVertex*)malloc(sizeof (VoxelVertex) * SECTOR_MESH_INITIAL_VERTEX_CAPACITY);
        sector_mesh->index_array = (u32*)malloc(sizeof(u32) * SECTOR_MESH_INITIAL_VERTEX_CAPACITY * 6 / 4);    
}

    renderer->interface_image_gpu_mesh_count = 0;
    renderer->interface_image_gpu_mesh_capacity = 10;
    renderer->interface_image_gpu_mesh_cache_array = (GpuMesh*)malloc(sizeof (GpuMesh) * renderer->interface_image_gpu_mesh_capacity);

    return renderer;
}

bool renderer_init(Renderer* renderer)
{
    u32 world_volume_in_sectors = get_world_volume_in_sectors();

    renderer->voxel_version = 0;
    renderer->character_version = 0;

    for (u32 sector_index = 0; sector_index < world_volume_in_sectors; ++sector_index)
    {
        SectorMesh* sector_mesh = &renderer->sector_mesh_cache_array[sector_index];
        sector_mesh->version = 0;
        
        ivec3 sector_coordinate, grid_coordinate;
        vec3 sector_world_position;
        mat4 model_matrix;

        sector_index_to_sector_coordinate(sector_index, sector_coordinate);
        sector_coordinate_to_grid_coordinate(sector_coordinate, grid_coordinate);
        grid_coordinate_to_world_position(grid_coordinate, sector_world_position);

        glm_mat4_identity(model_matrix);
        glm_translate(model_matrix, sector_world_position);

        GpuMesh* sector_gpu_mesh = &renderer->sector_gpu_mesh_cache_array[sector_index];
        sector_gpu_mesh->version = 0;
        sector_gpu_mesh->vbo_id = 0;
        sector_gpu_mesh->ebo_id = 0;

        glm_mat4_copy(model_matrix, sector_gpu_mesh->model_matrix);
    }

    renderer->texture_array_id = 0;

    stbi_set_flip_vertically_on_load(true);

    renderer_load_texture_array(renderer, "assets/textures");

    renderer->world_pipeline = {
        .program_id = 0,
        .projection_matrix_location = 0,
        .view_matrix_location = 0,
        .model_matrix_location = 0,
        .texture_sampler_location = 0,
        .vao_id = 0,
    };

    renderer->interface_pipeline = {
        .program_id = 0,
        .projection_matrix_location = 0,
        .view_matrix_location = 0,
        .model_matrix_location = 0,
        .texture_sampler_location = 0,
        .vao_id = 0
    };

    glm_ortho(
        0.0f,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0.0f,
        -1.0f,
        1.0f,
        renderer->interface_projection_matrix
    );

    for (u32 i = 0; i < renderer->interface_image_gpu_mesh_capacity; i++)
    {
        GpuMesh* mesh = &renderer->interface_image_gpu_mesh_cache_array[i];

        mesh->vbo_id = 0;
        mesh->ebo_id = 0;
        mesh->index_count = 0;
        mesh->version = 0;

        glm_mat4_identity(mesh->model_matrix);
    }

    renderer_setup_world_pipeline(renderer);
    renderer_setup_interface_pipeline(renderer);

    renderer_setup_world_shaders(renderer);
    renderer_setup_interface_shaders(renderer);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void renderer_destroy(Renderer* renderer)
{
    u32 world_volume_in_sectors = get_world_volume_in_sectors();

    for (u32 sector_index = 0; sector_index < world_volume_in_sectors; ++sector_index)
    {
        SectorMesh* sector_mesh = &renderer->sector_mesh_cache_array[sector_index];

        free(sector_mesh->vertex_data_array);
        free(sector_mesh->index_array);
    }

    free(renderer->sector_mesh_cache_array);
    free(renderer->sector_gpu_mesh_cache_array);

    free(renderer);
}

void renderer_load_texture_array(Renderer* renderer, const char* directory)
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
        NULL
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (u32 block_kind_index = 0; block_kind_index < BLOCK_KIND_COUNT; ++block_kind_index)
    {
        const char* filename = BLOCK_KIND_TEXTURE_FILENAME[block_kind_index];

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
            block_kind_index,
            width,
            height,
            1,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixel_data
        );

        stbi_image_free(pixel_data);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void renderer_setup_world_pipeline(Renderer* renderer)
{
    renderer->world_pipeline.program_id = glCreateProgram();

    glGenVertexArrays(1, &renderer->world_pipeline.vao_id);
}

void renderer_setup_world_shaders(Renderer* renderer)
{
    const char* world_vert_shader_src = shader_load("assets/shaders/world.vert");
    const char* world_frag_shader_src = shader_load("assets/shaders/world.frag");

    GLuint world_vert_shader_id = shader_compile(GL_VERTEX_SHADER, world_vert_shader_src);
    GLuint world_frag_shader_id = shader_compile(GL_FRAGMENT_SHADER, world_frag_shader_src);

    glAttachShader(renderer->world_pipeline.program_id, world_vert_shader_id);
    glAttachShader(renderer->world_pipeline.program_id, world_frag_shader_id);

    glLinkProgram(renderer->world_pipeline.program_id);

    int world_shader_linked = 0;
    glGetProgramiv(renderer->world_pipeline.program_id, GL_LINK_STATUS, &world_shader_linked);

    if (!world_shader_linked)
    {
        char log[1024];
        glGetProgramInfoLog(renderer->world_pipeline.program_id, sizeof(log), NULL, log);

        fprintf(stderr, "[PROGRAM LINK ERROR]\n%s\n", log);
    }

    glDeleteShader(world_vert_shader_id);
    glDeleteShader(world_frag_shader_id);

    free((void*)world_frag_shader_src);
    free((void*)world_vert_shader_src);

    renderer->world_pipeline.projection_matrix_location = glGetUniformLocation(renderer->world_pipeline.program_id, "u_projection_matrix");
    renderer->world_pipeline.view_matrix_location = glGetUniformLocation(renderer->world_pipeline.program_id, "u_view_matrix");
    renderer->world_pipeline.model_matrix_location = glGetUniformLocation(renderer->world_pipeline.program_id, "u_model_matrix");

    renderer->world_pipeline.texture_sampler_location = glGetUniformLocation(renderer->world_pipeline.program_id, "u_texture_sampler");

    if (renderer->world_pipeline.texture_sampler_location != -1) 
    {
        glUseProgram(renderer->world_pipeline.program_id);
        glUniform1i(renderer->world_pipeline.texture_sampler_location, 0);
    }
}

void renderer_setup_interface_pipeline(Renderer* renderer)
{
    renderer->interface_pipeline.program_id = glCreateProgram();

    glGenVertexArrays(1, &renderer->interface_pipeline.vao_id);
}

void renderer_setup_interface_shaders(Renderer* renderer)
{
    const char* interface_vert_shader_src = shader_load("assets/shaders/interface.vert");
    const char* interface_frag_shader_src = shader_load("assets/shaders/interface.frag");

    GLuint interface_vert_shader_id = shader_compile(GL_VERTEX_SHADER, interface_vert_shader_src);
    GLuint interface_frag_shader_id = shader_compile(GL_FRAGMENT_SHADER, interface_frag_shader_src);

    glAttachShader(renderer->interface_pipeline.program_id, interface_vert_shader_id);
    glAttachShader(renderer->interface_pipeline.program_id, interface_frag_shader_id);

    glLinkProgram(renderer->interface_pipeline.program_id);

    int interface_shader_linked = 0;
    glGetProgramiv(renderer->interface_pipeline.program_id, GL_LINK_STATUS, &interface_shader_linked);

    if (!interface_shader_linked)
    {
        char log[1024];
        glGetProgramInfoLog(renderer->interface_pipeline.program_id, sizeof(log), NULL, log);

        fprintf(stderr, "[PROGRAM LINK ERROR]\n%s\n", log);
    }

    glDeleteShader(interface_vert_shader_id);
    glDeleteShader(interface_frag_shader_id);

    free((void*)interface_vert_shader_src);
    free((void*)interface_frag_shader_src);

    renderer->interface_pipeline.projection_matrix_location = glGetUniformLocation(renderer->interface_pipeline.program_id, "u_projection_matrix");
    renderer->interface_pipeline.model_matrix_location = glGetUniformLocation(renderer->interface_pipeline.program_id, "u_model_matrix");

    renderer->interface_pipeline.texture_sampler_location = glGetUniformLocation(renderer->interface_pipeline.program_id, "u_texture_sampler");

    if (renderer->interface_pipeline.texture_sampler_location != -1) 
    {
        glUseProgram(renderer->interface_pipeline.program_id);
        glUniform1i(renderer->interface_pipeline.texture_sampler_location, 0);
    }
}

void renderer_update(Renderer* renderer, Interface* interface, World* world)
{
    if (renderer->voxel_version != world->voxel_version)
    {
        for (SectorIndex sector_index = 0; sector_index < get_world_volume_in_sectors(); sector_index++)
        {
            Sector* sector = &world->sector_array[sector_index];
    
            SectorMesh* sector_mesh = &renderer->sector_mesh_cache_array[sector_index];
            GpuMesh* sector_gpu_mesh = &renderer->sector_gpu_mesh_cache_array[sector_index];
    
            renderer_build_sector_mesh(renderer, sector, sector_mesh);
            renderer_upload_sector_mesh_to_gpu(renderer, sector_mesh, sector_gpu_mesh);
        }

        renderer->voxel_version = world->voxel_version;
    }

    if (renderer->character_version != world->character_version)
    {
        InterfaceImage* selected_block_image = &interface->interface_image_array[0];
        selected_block_image->texture_index = world->selected_block_kind;

        mat4 model;
        glm_mat4_identity(model);

        vec3 translate =
        {
            selected_block_image->position[0] + selected_block_image->size[0] * 0.5f,
            selected_block_image->position[1] + selected_block_image->size[1] * 0.5f,
            0.0f
        };

        glm_translate(model, translate);

        vec3 scale =
        {
            selected_block_image->size[0],
            selected_block_image->size[1],
            1.0f
        };

        glm_scale(model, scale);

        GpuMesh* gpu_mesh = &renderer->interface_image_gpu_mesh_cache_array[renderer->interface_image_gpu_mesh_count];

        glm_mat4_copy(model, gpu_mesh->model_matrix);

        renderer_upload_interface_image_to_gpu(
            renderer, 
            selected_block_image, 
            gpu_mesh
        );

        renderer->character_version = world->character_version;
    }
}

void renderer_build_sector_mesh(Renderer* renderer, Sector* sector, SectorMesh* out_sector_mesh)
{
    if (sector->version <= out_sector_mesh->version) 
    {
        return;
    }

    u32 sector_volume_in_cells = get_sector_volume_in_cells();
    
    out_sector_mesh->vertex_data_count = 0;
    out_sector_mesh->index_count = 0;

    for (CellIndex cell_index = 0; cell_index < sector_volume_in_cells; ++cell_index)
    {
        Cell* cell = &sector->cell_array[cell_index];

        if (cell->cell_face_mask == 0 || cell->block_kind == BLOCK_KIND_NONE)
        {
            continue;
        }

        CellCoordinate cell_coordinate;
        cell_index_to_cell_coordinate(cell_index, cell_coordinate);

        vec3 cell_sector_position;
        cell_sector_position[0] = (f32)cell_coordinate[0];
        cell_sector_position[1] = (f32)cell_coordinate[1];
        cell_sector_position[2] = (f32)cell_coordinate[2];

        for (u32 cell_face = 0; cell_face < CELL_FACE_COUNT; ++cell_face)
        {
            if (cell->cell_face_mask & CELL_FACE_BIT(cell_face))
            {
                renderer_emit_face(
                    renderer,
                    cell_sector_position, 
                    (CellFace)cell_face, 
                    cell->block_kind,
                    out_sector_mesh
                );
            }
        }
    }

    out_sector_mesh->version = sector->version;
}

void renderer_emit_face(Renderer* renderer, vec3 sector_position, CellFace cell_face, BlockKind block_kind, SectorMesh* sector_mesh)
{
    u32 base_index = sector_mesh->vertex_data_count;

    for (int vertex_index = 0; vertex_index < VERTICES_PER_FACE; ++vertex_index)
    {
        VoxelVertex vertex_data;

        glm_vec3_add(
            sector_position,
            FACE_VERTEX_ARRAY[cell_face][vertex_index],
            vertex_data.position
        );

        glm_vec3_copy(FACE_NORMAL_ARRAY[cell_face], vertex_data.normal);

        glm_vec2_copy(FACE_UV_ARRAY[cell_face][vertex_index], vertex_data.uv);

        vertex_data.texture_index = block_kind;

        sector_mesh->vertex_data_array[base_index + vertex_index] = vertex_data;
        sector_mesh->vertex_data_count += 1;
    }

    sector_mesh->index_array[sector_mesh->index_count + 0] = base_index + 0;
    sector_mesh->index_array[sector_mesh->index_count + 1] = base_index + 1;
    sector_mesh->index_array[sector_mesh->index_count + 2] = base_index + 2;
    sector_mesh->index_array[sector_mesh->index_count + 3] = base_index + 2;
    sector_mesh->index_array[sector_mesh->index_count + 4] = base_index + 3;
    sector_mesh->index_array[sector_mesh->index_count + 5] = base_index + 0;
    
    sector_mesh->index_count += 6;
}

void renderer_upload_sector_mesh_to_gpu(Renderer* renderer, SectorMesh* sector_mesh, GpuMesh* gpu_mesh)
{
    if (sector_mesh->version <= gpu_mesh->version)
    {
        return;
    }

    if (gpu_mesh->vbo_id == 0)
    {
        glGenBuffers(1, &gpu_mesh->vbo_id);
    }
    
    if (gpu_mesh->ebo_id == 0)
    {
        glGenBuffers(1, &gpu_mesh->ebo_id);
    }

    gpu_mesh->version = sector_mesh->version;
    gpu_mesh->index_count = sector_mesh->index_count;

    glBindVertexArray(renderer->world_pipeline.vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_id);

    glBufferData(
        GL_ARRAY_BUFFER,
        sector_mesh->vertex_data_count * sizeof(VoxelVertex),
        sector_mesh->vertex_data_array,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->ebo_id);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sector_mesh->index_count * sizeof(u32),
        sector_mesh->index_array,
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);
}

void renderer_upload_interface_image_to_gpu(Renderer* renderer, InterfaceImage* interface_image, GpuMesh* gpu_mesh)
{
    if (gpu_mesh->vbo_id == 0)
    {
        glGenBuffers(1, &gpu_mesh->vbo_id);
    }

    if (gpu_mesh->ebo_id == 0)
    {
        glGenBuffers(1, &gpu_mesh->ebo_id);
    }

    InterfaceVertex vertices[4];

    vertices[0].position[0] = -0.5f; vertices[0].position[1] = -0.5f;
    vertices[1].position[0] = +0.5f; vertices[1].position[1] = -0.5f;
    vertices[2].position[0] = +0.5f; vertices[2].position[1] = +0.5f;
    vertices[3].position[0] = -0.5f; vertices[3].position[1] = +0.5f;

    vertices[0].uv[0] = 0.0f; vertices[0].uv[1] = 1.0f;
    vertices[1].uv[0] = 1.0f; vertices[1].uv[1] = 1.0f;
    vertices[2].uv[0] = 1.0f; vertices[2].uv[1] = 0.0f;
    vertices[3].uv[0] = 0.0f; vertices[3].uv[1] = 0.0f;

    vertices[0].texture_index = interface_image->texture_index;
    vertices[1].texture_index = interface_image->texture_index;
    vertices[2].texture_index = interface_image->texture_index;
    vertices[3].texture_index = interface_image->texture_index;

    u32 indices[6] = {0, 1, 2, 2, 3, 0};

    gpu_mesh->index_count = 6;

    glBindVertexArray(renderer->interface_pipeline.vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void renderer_render(Renderer* renderer, Camera* camera) 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 projection_matrix, view_matrix;
    camera_get_projection_matrix(camera, projection_matrix);
    camera_get_view_matrix(camera, view_matrix);

    renderer_render_world_pipeline(renderer, projection_matrix, view_matrix);
    renderer_render_interface_pipeline(renderer);
}

void renderer_render_world_pipeline(Renderer* renderer, mat4 projection_matrix, mat4 view_matrix)
{
    glEnable(GL_DEPTH_TEST);

    glUseProgram(renderer->world_pipeline.program_id);
    
    glBindVertexArray(renderer->world_pipeline.vao_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderer->texture_array_id);

    glUniformMatrix4fv(
        renderer->world_pipeline.projection_matrix_location,
        1,
        GL_FALSE,
        (f32*)projection_matrix
    );

    glUniformMatrix4fv(
        renderer->world_pipeline.view_matrix_location,
        1,
        GL_FALSE,
        (f32*)view_matrix
    );

    for (SectorIndex sector_index = 0; sector_index < get_world_volume_in_sectors(); sector_index++)
    {
        GpuMesh* gpu_mesh = &renderer->sector_gpu_mesh_cache_array[sector_index];

        if (gpu_mesh->vbo_id == 0 || gpu_mesh->ebo_id == 0)
        {
            continue;
        }

        renderer_draw_world_gpu_mesh(renderer, gpu_mesh);
    }
}

void renderer_draw_world_gpu_mesh(Renderer* renderer, GpuMesh* gpu_mesh)
{
    glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->ebo_id);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertex),
        (void*)offsetof(VoxelVertex, position)
    );

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertex),
        (void*)offsetof(VoxelVertex, normal)
    );

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VoxelVertex),
        (void*)offsetof(VoxelVertex, uv)
    );

    glVertexAttribIPointer(
        3,
        1,
        GL_UNSIGNED_INT,
        sizeof(VoxelVertex),
        (void*)offsetof(VoxelVertex, texture_index)
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glUniformMatrix4fv(
        renderer->world_pipeline.model_matrix_location,
        1,
        GL_FALSE,
        (f32*)gpu_mesh->model_matrix
    );

    glDrawElements(
        GL_TRIANGLES,
        gpu_mesh->index_count,
        GL_UNSIGNED_INT,
        0
    );
}

void renderer_render_interface_pipeline(Renderer* renderer)
{
    glDisable(GL_DEPTH_TEST);

    glUseProgram(renderer->interface_pipeline.program_id);
    
    glBindVertexArray(renderer->interface_pipeline.vao_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderer->texture_array_id);

    glUniformMatrix4fv(
        renderer->interface_pipeline.projection_matrix_location,
        1,
        GL_FALSE,
        (f32*)renderer->interface_projection_matrix
    );

    renderer_draw_interface_gpu_mesh(renderer, &renderer->interface_image_gpu_mesh_cache_array[0]);
}

void renderer_draw_interface_gpu_mesh(Renderer* renderer, GpuMesh* gpu_mesh)
{
    if (gpu_mesh->vbo_id == 0 || gpu_mesh->ebo_id == 0)
    {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->ebo_id);

    glVertexAttribPointer(
        0, 
        2, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(InterfaceVertex),
        (void*)offsetof(InterfaceVertex, position)
    );

    glVertexAttribPointer(
        1, 
        2, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(InterfaceVertex),
        (void*)offsetof(InterfaceVertex, uv)
    );

    glVertexAttribIPointer(
        2, 
        1, 
        GL_UNSIGNED_INT,
        sizeof(InterfaceVertex),
        (void*)offsetof(InterfaceVertex, texture_index)
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glUniformMatrix4fv(
        renderer->interface_pipeline.model_matrix_location,
        1,
        GL_FALSE,
        (f32*)gpu_mesh->model_matrix
    );

    glDrawElements(
        GL_TRIANGLES,
        gpu_mesh->index_count,
        GL_UNSIGNED_INT,
        0
    );
}


