#version 410 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in uint a_texture_index;

uniform mat4 u_projection_matrix;
uniform mat4 u_model_matrix;

out vec2 v_uv;
flat out uint v_texture_index;

void main()
{
    gl_Position = u_projection_matrix * u_model_matrix * vec4(a_position, 0.0, 1.0);
    
    v_uv = a_uv;
    v_texture_index = a_texture_index;
}