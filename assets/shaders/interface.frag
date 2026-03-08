#version 410 core

in vec2 v_uv;
flat in uint v_texture_index;

uniform sampler2DArray u_texture_sampler;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_texture_sampler, vec3(v_uv, float(v_texture_index)));

    // FragColor = vec4(float(v_texture_index) / 5.0, 0, 0, 1);
}