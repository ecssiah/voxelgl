#pragma once

#include <cglm/cglm.h>

struct Camera
{
    float m_field_of_view = 60.0f;
    float m_aspect_ratio = 16.0f / 9.0f;
    float m_near_plane = 0.1f;
    float m_far_plane = 1000.0f;

    mat4 m_projection_matrix;
    mat4 m_view_matrix;

    vec3 m_position;

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_pitch_limit = GLM_PI / 2.0f - 0.01f;

    float m_speed = 2.4f;
    float m_sensitivity = 0.4f;

    bool init();

    void set_perspective(float field_of_view, float aspect_ratio, float near_plane, float far_plane);

    void update(double dt);

    void get_view_matrix(mat4 out_view_matrix);
    void get_projection_matrix(mat4 out_projection_matrix);

    void get_position(vec3 out_position);
    void set_position(float x, float y, float z);

    float get_yaw();
    void set_yaw(float yaw);

    float get_pitch();
    void set_pitch(float pitch);

    void get_forward(vec3 out_forward);
    void get_right(vec3 out_right);
    void get_up(vec3 out_up);

    void rebuild_projection_matrix();
    void rebuild_view_matrix();
};