#ifndef GRAPHICS_VERTEX_H
#define GRAPHICS_VERTEX_H

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoords;
};

struct VertexN
{
    glm::vec3 pos;
    glm::vec2 texCoords;
    glm::vec3 normal;
};

#endif
