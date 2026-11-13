#ifndef OPENGLGP_PROJECT_MODEL_H
#define OPENGLGP_PROJECT_MODEL_H

#include "project/Mesh.h"
#include "project/GLShaderProgram.h"
#include "project/GLTexture2D.h"
#include <string>
#include <vector>

struct aiNode;
struct aiScene;
struct aiMesh;

namespace openglgp::project
{
    class Model
    {
    public:
        void loadFromFile(const std::string& path);
        void draw(const gl::GLShaderProgram& shader, const glm::mat4& model) const;

        void drawParts(const gl::GLShaderProgram& shader,
            const glm::mat4& model,
            const std::vector<unsigned int>& parts) const;

        unsigned int partCount() const
        {
            return static_cast<unsigned int>(m_parts.size());
        }

        const std::string& partMaterial(unsigned int index) const;

    private:
        struct Material
        {
            std::string name;
            glm::vec3 color{ 1.0f };
            int texture = -1;
        };

        struct Part
        {
            Mesh mesh;
            unsigned int material;
            glm::mat4 transform;
        };

        void processNode(const aiNode* node, const aiScene* scene,
            const glm::mat4& parent);

        Mesh processMesh(const aiMesh* mesh);

        void drawPart(const gl::GLShaderProgram& shader,
            const glm::mat4& model, unsigned int index) const;

        std::vector<Material> m_materials;
        std::vector<gl::GLTexture2D> m_textures;
        std::vector<Part> m_parts;
    };
}

#endif