#ifndef OPENGLGP_PROJECT_MESH_H
#define OPENGLGP_PROJECT_MESH_H

#include "project/GLBuffer.h"
#include "project/GLVertexArray.h"
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

namespace openglgp::project
{
    struct MeshVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<MeshVertex>& vertices, const std::vector<std::uint32_t>& indices);
        void draw() const;

    private:
        gl::GLBuffer m_vbo, m_ebo;
        gl::GLVertexArray m_vao;
        int m_indexCount = 0;
    };
}

#endif