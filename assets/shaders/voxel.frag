#version 410 core

in vec3 v_normal;
in vec2 v_uv;
flat in uint v_texture_index;

uniform sampler2DArray u_texture_sampler;

out vec4 FragColor;

void main()
{
    vec4 texture_color = texture(u_texture_sampler, vec3(v_uv, float(v_texture_index)));

    FragColor = texture_color;
}