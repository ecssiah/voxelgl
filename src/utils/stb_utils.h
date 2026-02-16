#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

namespace stb_utils
{

inline GLuint load_texture_2d(const char* filename)
{
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* pixel_data = 
        stbi_load(
            filename,
            &width,
            &height,
            &channels,
            4
        );

    if (!pixel_data) 
    {
        std::cerr << "Failed to load image: " << filename << "\n";

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

}