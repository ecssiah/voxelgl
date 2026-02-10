#pragma once

#include <cglm/cglm.h>

class Camera
{

public:

    void set_perspective(float field_of_view, float aspect_ratio, float near_plane, float far_plane);

    void update(float dt);

    mat4& get_view_matrix();
    mat4& get_projection_matrix();

    void set_position(float x, float y, float z);

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

    float m_yaw;
    float m_pitch;
};