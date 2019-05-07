#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLushort> &indices)
{
    if (indices.empty()) {
        m_elCount = vertices.size();
    } else {
        m_elCount = indices.size();
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), 
            vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
            reinterpret_cast<void *>(offsetof(Vertex, pos)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
            reinterpret_cast<void *>(offsetof(Vertex, texCoords)));

    if (!indices.empty()) {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]),
                indices.data(), GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<VertexN> &vertices, const std::vector<GLushort> &indices)
{
    if (indices.empty()) {
        m_elCount = vertices.size();
    } else {
        m_elCount = indices.size();
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), 
            vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
            reinterpret_cast<void *>(offsetof(VertexN, pos)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
            reinterpret_cast<void *>(offsetof(VertexN, texCoords)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
            reinterpret_cast<void *>(offsetof(VertexN, normal)));

    if (!indices.empty()) {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]),
                indices.data(), GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}


Mesh::Mesh(Mesh &&other) :
    m_vao(other.m_vao),
    m_vbo(other.m_vbo),
    m_ebo(other.m_ebo),
    m_elCount(other.m_elCount)
{
    other.m_vao = other.m_vbo = other.m_ebo = 0;
}

Mesh::~Mesh()
{
    if (m_ebo) {
        glDeleteBuffers(1, &m_ebo);
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
    }
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
    }
}
    
void Mesh::Draw() const
{
    glBindVertexArray(m_vao);
    if (m_ebo) {
        glDrawElements(GL_TRIANGLES, m_elCount, GL_UNSIGNED_SHORT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_elCount);
    }
}
        

