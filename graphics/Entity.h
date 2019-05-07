#ifndef GRAPHICS_ENTITY_H
#define GRAPHICS_ENTITY_H

#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

#include <glm/glm.hpp>

class Entity
{
public:
    Entity(Mesh *mesh = nullptr, Shader *shader = nullptr, Texture *texture = nullptr);
    virtual ~Entity() = default;

    virtual void Update();
    virtual void Draw(const glm::mat4 &pv) const;

public:
    float                                           m_angle = 0.0;
    glm::vec3                                       m_rotAxis = {0.0, 0.0, 1.0};
    glm::vec3                                       m_position = {0.0, 0.0, 0.0};
    glm::vec3                                       m_scale = {1.0, 1.0, 1.0};

    Mesh *                                          m_mesh;
    Shader *                                        m_shader;
    Texture *                                       m_texture;
};

#endif
