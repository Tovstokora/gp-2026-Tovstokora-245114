#include "project/Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <stdexcept>

namespace openglgp::project
{
    void Model::loadFromFile(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure);

        if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
        {
            throw std::runtime_error("Cannot load model: " + path + ": " + importer.GetErrorString());
        }

        m_parts.clear();
        m_textures.clear();
        m_materials.clear();

        const std::filesystem::path directory = std::filesystem::path(path).parent_path();

        for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
        {
            const aiMaterial* source = scene->mMaterials[i];
            Material material;
            aiString materialName;
            source->Get(AI_MATKEY_NAME, materialName);
            material.name = materialName.C_Str();  
            aiColor3D color(1.0f, 1.0f, 1.0f);
            source->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            material.color = glm::vec3(color.r, color.g, color.b);
         
            if (source->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString name;
                if (source->GetTexture(aiTextureType_DIFFUSE, 0, &name) != AI_SUCCESS)
                {
                    throw std::runtime_error("Cannot read model texture path: " + path);
                }

                const std::filesystem::path texturePath = directory / name.C_Str();

                material.texture = static_cast<int>(m_textures.size());
                m_textures.push_back(gl::GLTexture2D::fromFile(
                    texturePath.string(), true, "model.texture"));
            }

            m_materials.push_back(material);
        }

        if (m_textures.empty()) throw std::runtime_error("Model has no base color texture. Check material files: " + path);

        processNode(scene->mRootNode, scene, glm::mat4(1.0f));

        if (m_parts.empty()) throw std::runtime_error("Model has no meshes: " + path);
    }

    void Model::processNode(const aiNode* node, const aiScene* scene, const glm::mat4& parent)
    {
        // Assimp zapisuje macierz wierszami, a konstruktor GLM przyjmuje kolumny.
        const aiMatrix4x4& matrix = node->mTransformation;
        const glm::mat4 local(
            matrix.a1, matrix.b1, matrix.c1, matrix.d1,
            matrix.a2, matrix.b2, matrix.c2, matrix.d2,
            matrix.a3, matrix.b3, matrix.c3, matrix.d3,
            matrix.a4, matrix.b4, matrix.c4, matrix.d4);

        const glm::mat4 transform = parent * local;

        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            if (mesh->mMaterialIndex >= m_materials.size())
                throw std::runtime_error("Invalid mesh material.");

            m_parts.push_back({ processMesh(mesh), mesh->mMaterialIndex, transform });
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            processNode(node->mChildren[i], scene, transform);
        }
    }

    Mesh Model::processMesh(const aiMesh* mesh)
    {
        if (!mesh->HasPositions() || !mesh->HasNormals() || !mesh->HasTextureCoords(0))
        {
            throw std::runtime_error(
                "Mesh must contain positions, normals and UV: " +
                std::string(mesh->mName.C_Str()));
        }

        std::vector<MeshVertex> vertices;
        std::vector<std::uint32_t> indices;

        vertices.reserve(mesh->mNumVertices);
        indices.reserve(static_cast<std::size_t>(mesh->mNumFaces) * 3);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            MeshVertex vertex;

            vertex.position = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z);

            vertex.normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z);

            vertex.uv = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y);

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];

            if (face.mNumIndices != 3)
                throw std::runtime_error("Model contains non-triangle geometry.");

            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        return Mesh(vertices, indices);
    }

    void Model::draw(const gl::GLShaderProgram& shader,
        const glm::mat4& model) const
    {
        for (unsigned int i = 0; i < m_parts.size(); ++i)
            drawPart(shader, model, i);
    }

    void Model::drawParts(const gl::GLShaderProgram& shader,
        const glm::mat4& model,
        const std::vector<unsigned int>& parts) const
    {
        for (unsigned int index : parts)
            drawPart(shader, model, index);
    }

    const std::string& Model::partMaterial(unsigned int index) const
    {
        return m_materials[m_parts.at(index).material].name;
    }

    void Model::drawPart(const gl::GLShaderProgram& shader,
        const glm::mat4& model, unsigned int index) const
    {
        const Part& part = m_parts.at(index);
        const Material& material = m_materials[part.material];

        shader.setMat4("uModel", model * part.transform);
        shader.setVec3("uColor", material.color);
        shader.setInt("uHasTexture", material.texture >= 0);

        if (material.texture >= 0)
            m_textures[material.texture].bind(0);

        part.mesh.draw();
    }
}