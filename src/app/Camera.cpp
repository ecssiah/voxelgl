#include "Camera.h"
#include "platform/InputSystem.h"
#include "utils/cglm_utils.h"


void Camera::set_perspective(float field_of_view, float aspect_ratio, float near_plane, float far_plane) 
{
    m_field_of_view = field_of_view;
    m_aspect_ratio = aspect_ratio;
    m_near_plane = near_plane;
    m_far_plane = far_plane;

    rebuild_projection_matrix();
}

void Camera::rebuild_view_matrix()
{
    vec3 forward;
    get_forward(forward);

    vec3 target;
    glm_vec3_add(m_position, forward, target);

    vec3 up;
    glm_vec3_copy(GLM_YUP, up);

    glm_lookat(
        m_position, 
        target, 
        up,
        m_view_matrix
    );
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

void Camera::get_view_matrix(mat4 out_view_matrix) const
{
    std::memcpy(out_view_matrix, m_view_matrix, sizeof(mat4));
}

void Camera::get_projection_matrix(mat4 out_projection_matrix) const
{
    std::memcpy(out_projection_matrix, m_projection_matrix, sizeof(mat4));
}

void Camera::get_position(vec3 out_position) const
{
    std::memcpy(out_position, m_position, sizeof(vec3));
}

void Camera::set_position(float x, float y, float z)
{
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

float Camera::get_yaw() const
{
    return glm_deg(m_yaw);
}

float Camera::get_pitch() const
{
    return glm_deg(m_pitch);
}

void Camera::get_forward(vec3 out_forward) const 
{
    out_forward[0] = cosf(m_yaw) * cosf(m_pitch);
    out_forward[1] = sinf(m_pitch);
    out_forward[2] = sinf(m_yaw) * cosf(m_pitch);

    cglm_utils::vec3_normalize_safe(out_forward);
}

void Camera::get_right(vec3 out_right) const 
{
    vec3 forward;
    get_forward(forward);

    vec3 up;
    glm_vec3_copy(GLM_YUP, up);

    glm_vec3_cross(forward, up, out_right);
    cglm_utils::vec3_normalize_safe(out_right);
}

void Camera::get_up(vec3 out_up) const
{
    vec3 forward, right;

    get_forward(forward);
    get_right(right);

    glm_vec3_cross(right, forward, out_up);
    glm_vec3_normalize(out_up);
}

void Camera::set_yaw(float yaw)
{
    m_yaw = glm_rad(yaw);
}

void Camera::set_pitch(float pitch)
{
    m_pitch = glm_rad(pitch);
}

void Camera::update(double dt)
{
    vec3 input_value = GLM_VEC3_ZERO_INIT;

    if (InputSystem::is_key_down(GLFW_KEY_W))
    {
        input_value[2] += 1.0f;
    }

    if (InputSystem::is_key_down(GLFW_KEY_S))
    {
        input_value[2] -= 1.0f;
    }

    if (InputSystem::is_key_down(GLFW_KEY_A))
    {
        input_value[0] -= 1.0f;
    }

    if (InputSystem::is_key_down(GLFW_KEY_D))
    {
        input_value[0] += 1.0f;
    }

    cglm_utils::vec3_normalize_safe(input_value);

    vec3 right, forward;
    get_forward(forward);
    get_right(right);

    vec3 flat_forward;
    glm_vec3_copy(forward, flat_forward);
    flat_forward[1] = 0.0f;

    cglm_utils::vec3_normalize_safe(flat_forward);

    vec3 delta_position;
    glm_vec3_scale(flat_forward, input_value[2], flat_forward);
    glm_vec3_scale(right, input_value[0], right);
    glm_vec3_add(flat_forward, right, delta_position);

    cglm_utils::vec3_normalize_safe(delta_position);

    glm_vec3_scale(delta_position, dt * m_speed, delta_position);
    glm_vec3_add(m_position, delta_position, m_position);

    m_yaw += dt * m_sensitivity * InputSystem::get_mouse_dx();
    m_pitch -= dt * m_sensitivity * InputSystem::get_mouse_dy();

    if (m_pitch > m_pitch_limit) 
    {
        m_pitch = m_pitch_limit;
    }

    if (m_pitch < -m_pitch_limit) 
    {
        m_pitch = -m_pitch_limit;
    }

    rebuild_view_matrix();
}