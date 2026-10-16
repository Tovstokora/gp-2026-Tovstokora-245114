#ifndef OPENGLGP_PROJECT_CAMERA_H
#define OPENGLGP_PROJECT_CAMERA_H

#include <glm/glm.hpp>

struct GLFWwindow;

namespace openglgp::project
{

    class Camera
    {
    public:
        void init(GLFWwindow* w);
        void update(GLFWwindow* w, float dt, bool uiWantsMouse, bool uiWantsKeyboard);
        void reset();
        glm::mat4 viewMatrix() const;
        glm::mat4 projectionMatrix(float aspect) const;
        float fov()   const { return m_fov; }
        float speed() const { return m_speed; }
        void setFov(float f) { m_fov = f; }
        void setSpeed(float s) { m_speed = s; }
    private:
        void updateVectors();
        void processKeyboard(GLFWwindow* w, float dt);
        void processMouse(GLFWwindow* w);
        glm::vec3 m_pos{ 0, 20, 40 }, m_front{ 0, 0, -1 }, m_up{ 0, 1, 0 }, m_right{ 1, 0, 0 }, m_worldUp{ 0, 1, 0 };
        float m_yaw = -90.0f, m_pitch = -20.0f;
        float m_speed = 20.0f, m_mouseSens = 0.12f;
        float m_fov = 60.0f, m_near = 0.1f, m_far = 500.0f;
        bool m_cursorCaptured = false, m_firstMouse = true;
        double m_lastX = 0.0, m_lastY = 0.0;
    };

}
#endif