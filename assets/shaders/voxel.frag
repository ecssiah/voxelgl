#version 410 core

in vec3 v_normal;
in vec2 v_uv;

uniform sampler2D u_texture_sampler;

out vec4 FragColor;

void main()
{
    vec3 normal_color = normalize(v_normal) * 0.5 + 0.5;
    vec3 texture_color = texture(u_texture_sampler, v_uv).rgb;

    // vec3 final_color = texture_color;
    // vec3 final_color = normal_color;
    // vec3 final_color = texture_color * normal_color;
    vec3 final_color = mix(texture_color, texture_color * normal_color, 0.7);

    FragColor = vec4(final_color, 1.0);
}