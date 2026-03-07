#include "camera.h"
#include "app/action/action.h"
#include "platform/input.h"
#include "platform/window.h"
#include "utils/cglm_utils.h"

Camera* camera_create()
{
    Camera* camera = (Camera*)malloc(sizeof (Camera));

    return camera;
}

bool camera_init(Camera* camera)
{
    camera_set_position(camera, 0.0f, 0.0f, 5.0f);
    camera_set_yaw(camera, -90.0f);
    camera_set_pitch(camera, 0.0f);

    camera_set_perspective(
        camera,
        glm_rad(60.0f),
        window_get_aspect_ratio(),
        0.1f,
        1000.0f
    );

    return true;
}

void camera_set_perspective(Camera* camera, f32 field_of_view, f32 aspect_ratio, f32 near_plane, f32 far_plane) 
{
    field_of_view = field_of_view;
    aspect_ratio = aspect_ratio;
    near_plane = near_plane;
    far_plane = far_plane;

    camera_rebuild_projection_matrix(camera);
}

void camera_rebuild_view_matrix(Camera* camera)
{
    vec3 forward;
    camera_get_forward(camera, forward);

    vec3 target;
    glm_vec3_add(camera->position, forward, target);

    vec3 up;
    glm_vec3_copy(GLM_YUP, up);

    glm_lookat(
        camera->position, 
        target, 
        up,
        camera->view_matrix
    );
}

void camera_rebuild_projection_matrix(Camera* camera)
{
    glm_perspective(
        camera->field_of_view,
        camera->aspect_ratio,
        camera->near_plane,
        camera->far_plane,
        camera->projection_matrix
    );
}

void camera_get_view_matrix(Camera* camera, mat4 out_view_matrix) 
{
    glm_mat4_copy(camera->view_matrix, out_view_matrix);
}

void camera_get_projection_matrix(Camera* camera, mat4 out_projection_matrix) 
{
    glm_mat4_copy(camera->projection_matrix, out_projection_matrix);
}

void camera_get_position(Camera* camera, vec3 out_position) 
{
    glm_vec3_copy(camera->position, out_position);
}

void camera_set_position(Camera* camera, f32 x, f32 y, f32 z)
{
    camera->position[0] = x;
    camera->position[1] = y;
    camera->position[2] = z;
}

f32 camera_get_yaw(Camera* camera)
{
    return glm_deg(camera->yaw);
}

f32 camera_get_pitch(Camera* camera) 
{
    return glm_deg(camera->pitch);
}

void camera_get_forward(Camera* camera, vec3 out_forward) 
{
    out_forward[0] = cosf(camera->yaw) * cosf(camera->pitch);
    out_forward[1] = sinf(camera->pitch);
    out_forward[2] = sinf(camera->yaw) * cosf(camera->pitch);

    cglm_ext_vec3_normalize_safe(out_forward);
}

void camera_get_right(Camera* camera, vec3 out_right)
{
    vec3 forward;
    camera_get_forward(camera, forward);

    vec3 up;
    glm_vec3_copy(GLM_YUP, up);

    glm_vec3_cross(forward, up, out_right);
    
    cglm_ext_vec3_normalize_safe(out_right);
}

void camera_get_up(Camera* camera, vec3 out_up)
{
    vec3 forward, right;

    camera_get_forward(camera, forward);
    camera_get_right(camera, right);

    glm_vec3_cross(right, forward, out_up);
    glm_vec3_normalize(out_up);
}

void camera_set_yaw(Camera* camera, f32 yaw)
{
    camera->yaw = glm_rad(yaw);
}

void camera_set_pitch(Camera* camera, f32 pitch)
{
    camera->pitch = glm_rad(pitch);
}

void camera_update(Camera* camera, Input* input, f64 dt)
{
    vec3 input_value = GLM_VEC3_ZERO_INIT;
    
    if (input_is_key_down(input, GLFW_KEY_D))
    {
        input_value[0] += 1.0f;
    }
    
    if (input_is_key_down(input, GLFW_KEY_A))
    {
        input_value[0] -= 1.0f;
    }

    if (input_is_key_down(input, GLFW_KEY_E))
    {
        input_value[1] += 1.0f;
    }

    if (input_is_key_down(input, GLFW_KEY_Q))
    {
        input_value[1] -= 1.0f;
    }
    
    if (input_is_key_down(input, GLFW_KEY_W))
    {
        input_value[2] += 1.0f;
    }
    
    if (input_is_key_down(input, GLFW_KEY_S))
    {
        input_value[2] -= 1.0f;
    }

    if (input_is_mouse_button_released(input, GLFW_MOUSE_BUTTON_LEFT))
    {
        vec3 origin;
        glm_vec3_copy(camera->position, origin);

        vec3 direction;
        camera_get_forward(camera, direction);

        Action action =
        {
            .action_type = ACTION_PLACE
        };

        glm_vec3_copy(origin, action.place.origin);
        glm_vec3_copy(direction, action.place.direction);

        action_queue_insert(&input->action_queue, action);
    }

    if (input_is_mouse_button_released(input, GLFW_MOUSE_BUTTON_RIGHT))
    {
        vec3 origin;
        glm_vec3_copy(camera->position, origin);

        vec3 direction;
        camera_get_forward(camera, direction);

        Action action =
        {
            .action_type = ACTION_REMOVE
        };

        glm_vec3_copy(origin, action.remove.origin);
        glm_vec3_copy(direction, action.remove.direction);

        action_queue_insert(&input->action_queue, action);
    }

    cglm_ext_vec3_normalize_safe(input_value);

    vec3 right, forward;
    camera_get_right(camera, right);
    camera_get_forward(camera, forward);

    vec3 unit_y = GLM_VEC3_ZERO_INIT;
    unit_y[1] = 1.0f;

    vec3 forward_xy;
    glm_vec3_copy(forward, forward_xy);
    forward_xy[1] = 0.0f;

    cglm_ext_vec3_normalize_safe(forward_xy);

    glm_vec3_scale(right, input_value[0], right);
    glm_vec3_scale(unit_y, input_value[1], unit_y);
    glm_vec3_scale(forward_xy, input_value[2], forward_xy);
    
    vec3 delta_position = GLM_VEC3_ZERO_INIT;
    glm_vec3_add(delta_position, right, delta_position);
    glm_vec3_add(delta_position, forward_xy, delta_position);
    glm_vec3_add(delta_position, unit_y, delta_position);

    cglm_ext_vec3_normalize_safe(delta_position);

    glm_vec3_scale(delta_position, dt * camera->speed, delta_position);
    glm_vec3_add(camera->position, delta_position, camera->position);

    camera->yaw += dt * camera->sensitivity * input_get_mouse_dx(input);
    camera->pitch -= dt * camera->sensitivity * input_get_mouse_dy(input);

    if (camera->pitch > camera->pitch_limit) 
    {
        camera->pitch = camera->pitch_limit;
    }

    if (camera->pitch < -camera->pitch_limit) 
    {
        camera->pitch = -camera->pitch_limit;
    }

    camera_rebuild_view_matrix(camera);
}