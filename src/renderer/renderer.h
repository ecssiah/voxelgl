#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "app/world/world.h"

struct SectorMesh;
struct GpuMesh;
struct Interface;
struct InterfaceImage;

struct Pipeline
{
    GLuint program_id;

    GLint projection_matrix_location;
    GLint view_matrix_location;
    GLint model_matrix_location;

    GLint texture_sampler_location;

    GLuint vao_id;
};

struct Renderer
{
    u32 voxel_version;
    u32 character_version;

    Pipeline world_pipeline;
    Pipeline interface_pipeline;

    GLuint texture_array_id;

    mat4 interface_projection_matrix;

    SectorMesh* sector_mesh_cache_array;
    GpuMesh* sector_gpu_mesh_cache_array;

    GpuMesh* interface_image_gpu_mesh_cache_array;
    u32 interface_image_gpu_mesh_count;
    u32 interface_image_gpu_mesh_capacity;
};

Renderer* renderer_create();
bool renderer_init(Renderer* renderer);
void renderer_update(Renderer* renderer, Interface* interface, World* world);
void renderer_destroy(Renderer* renderer);

void renderer_load_texture_array(Renderer* renderer, const char* directory);

void renderer_setup_world_pipeline(Renderer* renderer);
void renderer_setup_world_shaders(Renderer* renderer);
void renderer_setup_interface_pipeline(Renderer* renderer);
void renderer_setup_interface_shaders(Renderer* renderer);

void renderer_render(Renderer* renderer, Camera* camera);
void renderer_render_world_pipeline(Renderer* renderer, mat4 projection_matrix, mat4 view_matrix);
void renderer_render_interface_pipeline(Renderer* renderer);

void renderer_draw_world_gpu_mesh(Renderer* renderer, GpuMesh* gpu_mesh);
void renderer_draw_interface_gpu_mesh(Renderer* renderer, GpuMesh* gpu_mesh);

void renderer_build_sector_mesh(Renderer* renderer, Sector* sector, SectorMesh* out_sector_mesh);
void renderer_emit_face(Renderer* renderer, vec3 sector_position, CellFace cell_face, BlockKind cell_block_kind, SectorMesh* sector_mesh);

void renderer_upload_sector_mesh_to_gpu(Renderer* renderer, SectorMesh* sector_mesh, GpuMesh* gpu_mesh);
void renderer_upload_interface_image_to_gpu(Renderer* renderer, InterfaceImage* interface_image, GpuMesh* gpu_mesh);
