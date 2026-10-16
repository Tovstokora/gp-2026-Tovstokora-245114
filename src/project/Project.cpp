#include "project/Project.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace openglgp::project
{

    void Project::ensureInitialized(const framework::FrameContext& frame)
    {
        if (m_initialized) return;

        m_camera.init(frame.window);
        m_shader = gl::GLShaderProgram::fromFiles(
            "res/shaders/terrain.vert",
            "res/shaders/terrain.frag",
            "terrain.program");

        m_terrain.initialize(m_generator, m_params);
        m_lastAppliedParams = m_params;

        m_initialized = true;
    }

    void Project::applyParamsIfChanged()
    {
        const TerrainParams& a = m_params;
        const TerrainParams& b = m_lastAppliedParams;

        const bool changed =
            a.amplitude != b.amplitude ||
            a.baseFrequency != b.baseFrequency ||
            a.frequencyGain != b.frequencyGain ||
            a.amplitudePersistence != b.amplitudePersistence ||
            a.octaves != b.octaves ||
            a.gridSize != b.gridSize ||
            a.worldSize != b.worldSize;

        if (changed)
        {
            m_terrain.regenerate(m_generator, m_params);
            m_lastAppliedParams = m_params;
        }
    }

    void Project::update(const framework::FrameContext& frame)
    {
        ensureInitialized(frame);
        m_camera.update(frame.window, frame.deltaSeconds,
            frame.uiWantsMouse, frame.uiWantsKeyboard);
    }

    void Project::render(const framework::FrameContext& frame)
    {
        ensureInitialized(frame);

        glViewport(0, 0, frame.framebufferWidth, frame.framebufferHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.55f, 0.68f, 0.82f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        const float aspect = static_cast<float>(frame.framebufferWidth) /
            static_cast<float>(frame.framebufferHeight);

        const glm::mat4 model(1.0f);
        const glm::mat4 view = m_camera.viewMatrix();
        const glm::mat4 proj = m_camera.projectionMatrix(aspect);

        m_shader.use();
        m_shader.setMat4("uModel", model);
        m_shader.setMat4("uView", view);
        m_shader.setMat4("uProjection", proj);

        m_terrain.draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Project::drawGui()
    {
        ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(380.0f, 360.0f), ImGuiCond_FirstUseEver);

        ImGui::Begin("Winter is coming - Tundra");

        ImGui::Text("Seed: %u", m_generator.seed());
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::SliderFloat("Amplitude", &m_params.amplitude, 0.5f, 20.0f);
        ImGui::SliderFloat("Frequency", &m_params.baseFrequency, 0.01f, 0.5f);
        ImGui::SliderFloat("Freq gain", &m_params.frequencyGain, 1.1f, 3.0f);
        ImGui::SliderFloat("Persistence", &m_params.amplitudePersistence, 0.1f, 0.9f);
        ImGui::SliderInt("Octaves", &m_params.octaves, 1, 8);
        ImGui::SliderInt("Grid size", &m_params.gridSize, 8, 256);
        ImGui::SliderFloat("World size", &m_params.worldSize, 20.0f, 300.0f);

        ImGui::Separator();
        ImGui::Text("Mesh: %d vertices, %d triangles, %d indices",
            m_terrain.vertexCount(),
            m_terrain.triangleCount(),
            m_terrain.indexCount());

        ImGui::Separator();
        ImGui::Checkbox("Wireframe", &m_wireframe);
        ImGui::SameLine();
        if (ImGui::Button("Reset params")) m_params = TerrainParams{};

        ImGui::Separator();
        float spd = m_camera.speed();
        if (ImGui::SliderFloat("Camera speed", &spd, 1.0f, 80.0f)) m_camera.setSpeed(spd);
        float fov = m_camera.fov();
        if (ImGui::SliderFloat("FOV", &fov, 20.0f, 100.0f)) m_camera.setFov(fov);
        if (ImGui::Button("Reset camera")) m_camera.reset();

        ImGui::Separator();
        ImGui::TextWrapped("Click window to capture mouse. Esc to release. WASD move, Space/Ctrl up/down.");

        ImGui::End();

        applyParamsIfChanged();
    }

} // namespace openglgp::project