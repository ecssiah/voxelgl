#pragma once

#include <cglm/cglm.h>

#include "app/world/world.h"
#include "core/types.h"
#include "platform/input.h"

struct Input;

struct Camera
{
    f32 field_of_view;
    f32 aspect_ratio;
    f32 near_plane;
    f32 far_plane;

    mat4 projection_matrix;
    mat4 view_matrix;

    vec3 position;

    f32 yaw;
    f32 pitch;
    f32 pitch_limit;

    f32 speed;
    f32 sensitivity;

    // Testing Only
    BlockKind block_kind_selected;
    BlockKind block_kind_selection_array[4];
};

Camera* camera_create();
bool camera_init(Camera* camera);

void camera_set_perspective(Camera* camera, f32 field_of_view, f32 aspect_ratio, f32 near_plane, f32 far_plane);

void camera_update(Camera* camera, Input* input, f64 dt);

void camera_get_view_matrix(Camera* camera, mat4 out_view_matrix);
void camera_get_projection_matrix(Camera* camera, mat4 out_projection_matrix);

void camera_get_position(Camera* camera, vec3 out_position);
void camera_set_position(Camera* camera, f32 x, f32 y, f32 z);

f32 camera_get_yaw(Camera* camera);
void camera_set_yaw(Camera* camera, f32 yaw);

f32 camera_get_pitch(Camera* camera);
void camera_set_pitch(Camera* camera, f32 pitch);

void camera_get_forward(Camera* camera, vec3 out_forward);
void camera_get_right(Camera* camera, vec3 out_right);
void camera_get_up(Camera* camera, vec3 out_up);

void camera_rebuild_projection_matrix(Camera* camera);
void camera_rebuild_view_matrix(Camera* camera);