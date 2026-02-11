#pragma once

#include <cglm/cglm.h>

class Camera
{

public:

    void set_perspective(float field_of_view, float aspect_ratio, float near_plane, float far_plane);

    void update(float dt);

    mat4& get_view_matrix();
    mat4& get_projection_matrix();

    vec3& get_position();
    void set_position(float x, float y, float z);

    float get_yaw() const;
    float get_pitch() const;

    void get_forward(vec3 out_forward) const;
    void get_right(vec3 out_right) const;
    void get_up(vec3 out_up) const;

    void set_yaw(float yaw);
    void set_pitch(float pitch);

private:

    void rebuild_projection_matrix();
    void rebuild_view_matrix();

    float m_field_of_view;
    float m_aspect_ratio;
    float m_near_plane;
    float m_far_plane;

    mat4 m_projection_matrix;
    mat4 m_view_matrix;

    vec3 m_position;

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_pitch_limit = GLM_PI / 2.0f - 0.01f;

    float m_speed = 1.5f;
    float m_sensitivity = 0.4f;

};