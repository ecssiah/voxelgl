#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in uint a_texture_index;

uniform mat4 u_mvp_matrix;

out vec3 v_normal;
out vec2 v_uv;
flat out uint v_texture_index;

void main() {
    gl_Position = u_mvp_matrix * vec4(a_position, 1.0);

    v_normal = a_normal;
    v_uv = a_uv;
    v_texture_index = a_texture_index;
}