#include "Entity.h"
#include "../App.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Entity::Entity(Mesh *mesh, Shader *shader, Texture *texture) :
    m_mesh(mesh),
    m_shader(shader),
    m_texture(texture)
{
}

void Entity::Update()
{
}

void Entity::Draw(const glm::mat4 &pv) const
{
    if (!m_mesh || !m_shader) {
        std::cerr << "Error: drawing without mesh or shader!" << std::endl;
        return;
    }
    glm::mat4 t(1.0);
    t = glm::translate(t, m_position);
    t = glm::scale(t, m_scale);
    t = glm::rotate(t, m_angle, m_rotAxis);
    if (m_shader == &App::app->m_shaders[App::SHADER_LIGHTING]) {
        m_shader->SetUniform("modelTransform", t);
        if (m_texture) {
            m_shader->SetUniform("basicColor", glm::vec3(0.0, 0.0, 0.0));
        } else {
            m_shader->SetUniform("basicColor", m_basicColor);
        }
    }

    m_shader->SetUniform("fullTransform", pv * t);
    m_shader->Use();
    if (m_texture) {
        m_texture->Bind();
    }
    m_mesh->Draw();
}
