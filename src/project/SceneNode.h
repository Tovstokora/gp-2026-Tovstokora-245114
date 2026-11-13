#ifndef OPENGLGP_PROJECT_SCENENODE_H
#define OPENGLGP_PROJECT_SCENENODE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

namespace openglgp::project
{
    struct SceneNode
    {
        std::string name;
        SceneNode* parent = nullptr;

        glm::vec3 position{ 0.0f };
        glm::vec3 rotation{ 0.0f };
        float animationRotationY = 0.0f;
        glm::vec3 scale{ 1.0f };
        glm::vec3 pivot{ 0.0f };
        std::vector<unsigned int> parts{};

        glm::mat4 localMatrix() const
        {
            glm::mat4 matrix(1.0f);

            matrix = glm::translate(matrix, position);
            matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix,
                glm::radians(rotation.y + animationRotationY),
                glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, scale);

            return matrix;
        }

        glm::mat4 worldMatrix() const
        {
            if (parent) return parent->worldMatrix() * localMatrix();

            return localMatrix();
        }
    };
}

#endif