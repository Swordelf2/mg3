#include "Background.h"

Background::Background(Mesh *mesh, Shader *shader, Texture *texture) :
    Entity(mesh, shader, texture)
{
}

void Background::Draw() const
{
    m_shader->Use();
    m_texture->Bind();
    m_mesh->Draw();
}