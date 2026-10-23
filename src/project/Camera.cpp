#include "project/Camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>


namespace openglgp::project
{

    void Camera::init()
    {
        updateVectors();
    }

    void Camera::reset()
    {
        m_pos = glm::vec3(0.0f, 20.0f, 40.0f);
        m_yaw = -90.0f;
        m_pitch = -20.0f;
        updateVectors();
    }

    void Camera::update(GLFWwindow* window, float deltaSeconds, bool uiWantsMouse, bool uiWantsKeyboard)
    {
        if (!m_cursorCaptured && !uiWantsMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            m_cursorCaptured = true;
            m_firstMouse = true;
        }
        if (m_cursorCaptured && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_cursorCaptured = false;
        }
        if (m_cursorCaptured && !uiWantsMouse) processMouse(window);
        if (!uiWantsKeyboard) processKeyboard(window, deltaSeconds);
    }

    void Camera::processMouse(GLFWwindow* window)
    {
        double x = 0.0, y = 0.0;
        glfwGetCursorPos(window, &x, &y);
        if (m_firstMouse) { m_lastX = x; m_lastY = y; m_firstMouse = false; }
        const float dx = static_cast<float>(x - m_lastX) * m_mouseSens;
        const float dy = static_cast<float>(m_lastY - y) * m_mouseSens;
        m_lastX = x; m_lastY = y;
        m_yaw += dx;
        m_pitch = std::clamp(m_pitch + dy, -89.0f, 89.0f);
        updateVectors();
    }

    void Camera::processKeyboard(GLFWwindow* window, float dt)
    {
        const float v = m_speed * dt;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_pos += m_front * v;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_pos -= m_front * v;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_pos -= m_right * v;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_pos += m_right * v;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) m_pos += m_worldUp * v;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) m_pos -= m_worldUp * v;
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) reset();
    }

    void Camera::updateVectors()
    {
        const float yr = glm::radians(m_yaw);
        const float pr = glm::radians(m_pitch);
        glm::vec3 f;
        f.x = std::cos(yr) * std::cos(pr);
        f.y = std::sin(pr);
        f.z = std::sin(yr) * std::cos(pr);
        m_front = glm::normalize(f);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

    glm::mat4 Camera::viewMatrix() const
    {
        return glm::lookAt(m_pos, m_pos + m_front, m_up);
    }

    glm::mat4 Camera::projectionMatrix(float aspect) const
    {
        return glm::perspective(glm::radians(m_fov), aspect, m_near, m_far);
    }

}

