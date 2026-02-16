#include <glad/glad.h>
#include <iostream>

namespace gl_utils
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