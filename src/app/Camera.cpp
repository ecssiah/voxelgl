#include "Camera.h"

void Camera::set_perspective(float field_of_view, float aspect_ratio, float near_plane, float far_plane) 
{
    m_field_of_view = field_of_view;
    m_aspect_ratio = aspect_ratio;
    m_near_plane = near_plane;
    m_far_plane = far_plane;

    rebuild_projection_matrix();
}

void Camera::rebuild_projection_matrix()
{
    glm_perspective(
        m_field_of_view,
        m_aspect_ratio,
        m_near_plane,
        m_far_plane,
        m_projection_matrix
    );
}

void Camera::rebuild_view_matrix()
{
    vec3 forward = {
        cosf(m_yaw) * cosf(m_pitch),
        sinf(m_pitch),
        sinf(m_yaw) * cosf(m_pitch)
    };

    vec3 target_position;

    glm_vec3_add(m_position, forward, target_position);

    glm_lookat(
        m_position, 
        target_position, 
        (vec3){0.f, 1.f, 0.f}, 
        m_view_matrix
    );
}

mat4& Camera::get_view_matrix() {
    return m_view_matrix;
}

mat4& Camera::get_projection_matrix() {
    return m_projection_matrix;
}

void Camera::set_position(float x, float y, float z)
{
    glm_vec3_copy(
        (vec3){ x, y, z }, 
        m_position
    );

}

void Camera::set_yaw(float yaw)
{
    m_yaw = yaw;
}

void Camera::set_pitch(float pitch)
{
    m_pitch = pitch;
}

void Camera::update(float dt)
{
    rebuild_view_matrix();


}