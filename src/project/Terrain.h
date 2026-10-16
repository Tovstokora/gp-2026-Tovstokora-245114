#ifndef OPENGLGP_PROJECT_TERRAIN_H
#define OPENGLGP_PROJECT_TERRAIN_H

#include "project/TerrainGenerator.h"
#include "project/GLBuffer.h"
#include "project/GLVertexArray.h"
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

namespace openglgp::project
{

    struct TerrainVertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    class Terrain
    {
    public:
        void initialize(const TerrainGenerator& gen, const TerrainParams& params);
        void regenerate(const TerrainGenerator& gen, const TerrainParams& params);
        void draw() const;
        int vertexCount()   const { return m_vertexCount; }
        int triangleCount() const { return m_indexCount / 3; }
        int indexCount()    const { return m_indexCount; }
    private:
        void buildCpuData(const TerrainGenerator& gen);
        void uploadToGpu();
        TerrainParams m_params;
        std::vector<TerrainVertex> m_vertices;
        std::vector<std::uint32_t> m_indices;
        int m_vertexCount = 0, m_indexCount = 0;
        gl::GLBuffer m_vbo, m_ebo;
        gl::GLVertexArray m_vao;
    };

}
#endif