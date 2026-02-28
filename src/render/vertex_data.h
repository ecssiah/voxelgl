#include <cstdint>

struct VertexData
{
    float m_position[3];
    float m_normal[3];
    float m_uv[2];
    uint8_t m_texture_index;
};