#pragma once

#include <cglm/cglm.h>

#include "core/types.h"
#include "platform/input.h"

struct Input;

struct Camera
{
    f32 field_of_view = 60.0f;
    f32 aspect_ratio = 16.0f / 9.0f;
    f32 near_plane = 0.1f;
    f32 far_plane = 1000.0f;

    mat4 projection_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;

    vec3 position = GLM_VEC3_ZERO_INIT;

    f32 yaw = 0.0f;
    f32 pitch = 0.0f;
    f32 pitch_limit = GLM_PI / 2.0f - 0.01f;

    f32 speed = 8.0f;
    f32 sensitivity = 0.4f;
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