#include "project/Mesh.h"
#include <cstddef>
#include <stdexcept>

namespace openglgp::project
{
    Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<std::uint32_t>& indices)
    {
        if (vertices.empty() || indices.empty()) throw std::runtime_error("Mesh has no vertices or indices.");

        m_indexCount = static_cast<int>(indices.size());

        m_vbo = gl::GLBuffer(vertices.data(), vertices.size() * sizeof(MeshVertex), "mesh.VBO");
        m_ebo = gl::GLBuffer(indices.data(), indices.size() * sizeof(std::uint32_t), "mesh.EBO");

        m_vao.setLabel("mesh.VAO");
        m_vao.bindVertexBuffer(0, m_vbo.id(), 0, sizeof(MeshVertex));
        m_vao.bindElementBuffer(m_ebo.id());

        m_vao.attribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, position));
        m_vao.attribBinding(0, 0);
        m_vao.enableAttrib(0);

        m_vao.attribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, normal));
        m_vao.attribBinding(1, 0);
        m_vao.enableAttrib(1);

        m_vao.attribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, uv));
        m_vao.attribBinding(2, 0);
        m_vao.enableAttrib(2);
    }

    void Mesh::draw() const
    {
        m_vao.bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
        m_vao.unbind();
    }
}