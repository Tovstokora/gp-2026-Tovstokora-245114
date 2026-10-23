#include "project/Terrain.h"
#include <glad/glad.h>
#include <algorithm>
#include <cstddef>
#include <limits>


namespace openglgp::project
{
   
    namespace
    {
        
        glm::vec3 heatmapTundra(
            float height,
            float minHeight,
            float maxHeight)
        {
            
            const float range =
                std::max(maxHeight - minHeight, 1e-6f);

            
            const float t = std::clamp(
                (height - minHeight) / range,
                0.0f,
                1.0f);

            
            const glm::vec3 ice(0.55f, 0.72f, 0.85f);
            const glm::vec3 rock(0.30f, 0.33f, 0.38f);
            const glm::vec3 snow(0.95f, 0.97f, 1.00f);

           
            if (t < 0.5f)
                return glm::mix(ice, rock, t * 2.0f);

            
            return glm::mix(
                rock,
                snow,
                (t - 0.5f) * 2.0f);
        }
    }


    void Terrain::initialize(
        const TerrainGenerator& gen,
        const TerrainParams& params)
    {
        
        regenerate(gen, params);
    }

   
    void Terrain::regenerate(
        const TerrainGenerator& gen,
        const TerrainParams& params)
    {
        
        m_params = params;

       
        buildCpuData(gen);

       
        uploadToGpu();
    }

    
    void Terrain::buildCpuData(const TerrainGenerator& gen)
    {
        
        const auto N =
            static_cast<std::uint32_t>(m_params.gridSize);

      
        const float half = m_params.worldSize * 0.5f;

       
        const float step =
            m_params.worldSize / static_cast<float>(N - 1);

        m_vertices.resize(static_cast<std::size_t>(N) * N);

        float minHeight = std::numeric_limits<float>::max();
        float maxHeight = std::numeric_limits<float>::lowest();

       
        for (std::uint32_t j = 0; j < N; ++j)
        {
            
            for (std::uint32_t i = 0; i < N; ++i)
            {
               
                const float x =
                    -half + static_cast<float>(i) * step;

               
                const float z =
                    -half + static_cast<float>(j) * step;

               
                const float h = gen.height(x, z, m_params);

                
                TerrainVertex& vertex =
                    m_vertices[static_cast<std::size_t>(j) * N + i];

                vertex.position = glm::vec3(x, h, z);

                minHeight = std::min(minHeight, h);

                
                maxHeight = std::max(maxHeight, h);
            }
        }

       
        for (TerrainVertex& vertex : m_vertices)
        {
            
            vertex.color = heatmapTundra(
                vertex.position.y,
                minHeight,
                maxHeight);
        }

        
        m_indices.clear();

        const std::size_t cellsPerSide = static_cast<size_t>(N) - 1;

        m_indices.reserve(cellsPerSide * cellsPerSide * 6);
        for (std::uint32_t j = 0; j < N - 1; ++j)
        {
            for (std::uint32_t i = 0; i < N - 1; ++i)
            {
              
                const std::uint32_t a = j * N + i;
                const std::uint32_t b = a + 1;

                const std::uint32_t c = a + N;

                const std::uint32_t d = c + 1;

                m_indices.insert(
                    m_indices.end(),
                    { a, c, b, b, c, d });
            }
        }

        m_vertexCount = static_cast<int>(m_vertices.size());
        m_indexCount = static_cast<int>(m_indices.size());
    }

   
    void Terrain::uploadToGpu()
    {
        
        const std::size_t vboSize =
            m_vertices.size() * sizeof(TerrainVertex);

        const std::size_t eboSize =
            m_indices.size() * sizeof(std::uint32_t);

        m_vbo = gl::GLBuffer(
            m_vertices.data(), vboSize, "terrain.VBO");

        m_ebo = gl::GLBuffer(
            m_indices.data(), eboSize, "terrain.EBO");

        m_vao = gl::GLVertexArray();

        m_vao.setLabel("terrain.VAO");

     
        m_vao.bindVertexBuffer(
            0, m_vbo.id(), 0, sizeof(TerrainVertex));

        
        m_vao.bindElementBuffer(m_ebo.id());
 
        m_vao.attribFormat(
            0, 3, GL_FLOAT, GL_FALSE,
            offsetof(TerrainVertex, position));

        m_vao.attribBinding(0, 0);
  
        m_vao.enableAttrib(0);

        
        
        m_vao.attribFormat(
            1, 3, GL_FLOAT, GL_FALSE,
            offsetof(TerrainVertex, color));

       
        m_vao.attribBinding(1, 0);

        
        m_vao.enableAttrib(1);
    }

    
  
    void Terrain::draw() const
    {
        
        m_vao.bind();

       
        glDrawElements(
            GL_TRIANGLES,                      
            static_cast<GLsizei>(m_indexCount), 
            GL_UNSIGNED_INT,                   
            nullptr);                         

        
        m_vao.unbind();
    }

} 