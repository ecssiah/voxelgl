#include <glad/glad.h>
#include <iostream>
#include <cglm/cglm.h>
#include <stb_image.h>

namespace cglm
{

inline void normalize_vec3_safe(vec3 out_vector)
{
    if (glm_vec3_norm(out_vector) > 0.0f) 
    {
        glm_vec3_normalize(out_vector);
    }
    else
    {
        out_vector[0] = 0.0f;
        out_vector[1] = 0.0f;
        out_vector[2] = 0.0f;
    }
}

inline void print_vec3(vec3 vector)
{
    std::cout << 
        "vec3: (" << 
        vector[0] << " " << 
        vector[1] << " " << 
        vector[2] << 
        ")" << std::endl;
}

inline void print_mat4(const char* name, mat4 matrix)
{
    std::cout << name << ":\n";
    
    for (int row = 0; row < 4; ++row)
    {
        std::cout
            << matrix[0][row] << " "
            << matrix[1][row] << " "
            << matrix[2][row] << " "
            << matrix[3][row] << "\n";
    }

    std::cout << "\n";
}

} // namespace glm


namespace gl
{

inline void get_errors(const char* label) 
{
    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR) 
    {
        std::cerr << 
            "[GL ERROR] " << label << ": 0x" << 
            std::hex << err << std::dec << 
            "\n";
    }
}

} // namespace gl

namespace texture
{

inline GLuint load_texture_2d(const char* path)
{
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* pixel_data = 
        stbi_load(
            path,
            &width,
            &height,
            &channels,
            4
        );

    if (!pixel_data) 
    {
        std::cerr << "Failed to load image: " << path << "\n";

        return 0;
    }

    GLuint texture_id = 0;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixel_data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(pixel_data);

    return texture_id;
}

} // namespace texture