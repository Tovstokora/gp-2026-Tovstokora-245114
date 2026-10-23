#include "project/Project.h"
#include <glad/glad.h>
#include <imgui.h>
#include <cstdio>

#ifdef OPENGLGP_DEBUG_BUILD


namespace
{
    
    void APIENTRY projectDebugCallback(
        GLenum,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei,
        const GLchar* message,
        const void*) noexcept
    {
        
        const bool isError =
            type == GL_DEBUG_TYPE_ERROR ||
            severity == GL_DEBUG_SEVERITY_HIGH;

        
        if (!isError &&
            severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        {
            return;
        }

        
        std::fprintf(
            stderr,
            "OpenGL %s [id=%u]: %s\n",
            isError ? "blad" : "komunikat",
            id,
            message);
    }
}

#endif

namespace openglgp::project
{
    void Project::ensureInitialized()
    {
        
        if (m_initialized)
        {
            return;
        }

#ifdef OPENGLGP_DEBUG_BUILD
      
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(projectDebugCallback, nullptr);
#endif

        
        m_camera.init();
        m_shader = gl::GLShaderProgram::fromFiles(
            "res/shaders/terrain.vert",
            "res/shaders/terrain.frag",
            "terrain.program");

        m_terrain.initialize(m_generator, m_params);

        m_initialized = true;
    }

    void Project::update(const framework::FrameContext& frame)
    {
        
        ensureInitialized();

        m_camera.update(
            frame.window,
            static_cast<float>(frame.deltaSeconds),
            frame.uiWantsMouse,
            frame.uiWantsKeyboard);
    }

    void Project::render(const framework::FrameContext& frame)
    {
        
        ensureInitialized();

        
        glViewport(
            0,
            0,
            frame.framebufferWidth,
            frame.framebufferHeight);

        
        glEnable(GL_DEPTH_TEST);
        
        glClearColor(0.55f, 0.68f, 0.82f, 1.0f);
                
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (m_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

       
        const float aspect =
            static_cast<float>(frame.framebufferWidth) /
            static_cast<float>(frame.framebufferHeight);

       
        const glm::mat4 model(1.0f);

      
        const glm::mat4 view = m_camera.viewMatrix();

        const glm::mat4 projection =
            m_camera.projectionMatrix(aspect);

        
        m_shader.use();      
        m_shader.setMat4("uModel", model);
        m_shader.setMat4("uView", view);
        m_shader.setMat4("uProjection", projection);        
        m_terrain.draw();
        if (m_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    void Project::drawGui()
    {
        
        ImGui::SetNextWindowPos(
            ImVec2(16.0f, 16.0f),
            ImGuiCond_FirstUseEver);

        ImGui::SetNextWindowSize(
            ImVec2(380.0f, 360.0f),
            ImGuiCond_FirstUseEver);

        bool terrainChanged = false;

        if (ImGui::Begin("Tundra"))
        {
            
            ImGui::Text("Seed: %u", m_generator.seed());
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

            ImGui::Separator();

            
            terrainChanged |= ImGui::SliderFloat(
                "Amplitude",
                &m_params.amplitude,
                0.5f,
                20.0f);

           
            terrainChanged |= ImGui::SliderFloat(
                "Frequency",
                &m_params.baseFrequency,
                0.01f,
                0.5f);

            terrainChanged |= ImGui::SliderFloat(
                "Freq gain",
                &m_params.frequencyGain,
                1.1f,
                3.0f);

            
            terrainChanged |= ImGui::SliderFloat(
                "Persistence",
                &m_params.amplitudePersistence,
                0.1f,
                0.9f);

            
            terrainChanged |= ImGui::SliderInt(
                "Octaves",
                &m_params.octaves,
                1,
                8,
                "%d",
                ImGuiSliderFlags_AlwaysClamp);

            
            terrainChanged |= ImGui::SliderInt(
                "Grid size",
                &m_params.gridSize,
                64,
                256,
                "%d",
                ImGuiSliderFlags_AlwaysClamp);

            
            terrainChanged |= ImGui::SliderFloat(
                "World size",
                &m_params.worldSize,
                20.0f,
                300.0f);

            ImGui::Separator();
            ImGui::Text(
                "Mesh: %d vertices, %d triangles, %d indices",
                m_terrain.vertexCount(),
                m_terrain.triangleCount(),
                m_terrain.indexCount());

            ImGui::Separator();

            ImGui::Checkbox("Wireframe", &m_wireframe);           
            ImGui::SameLine();

            if (ImGui::Button("Reset params"))
            {
              
                m_params = TerrainParams{};

                
                terrainChanged = true;
            }

            ImGui::Separator();
            
            float speed = m_camera.speed();
           
            if (ImGui::SliderFloat(
                "Camera speed", &speed, 1.0f, 80.0f))
            {
                m_camera.setSpeed(speed);
            }

            
            float fov = m_camera.fov();

            if (ImGui::SliderFloat(
                "FOV",
                &fov,
                20.0f,
                100.0f,
                "%.1f",
                ImGuiSliderFlags_AlwaysClamp))
            {
                m_camera.setFov(fov);
            }

            
            if (ImGui::Button("Reset camera"))
            {
                m_camera.reset();
            }

            ImGui::Separator();

            ImGui::TextWrapped(
                "Click window to capture mouse. Esc to release. "
                "WASD move, Space/Ctrl up/down.");
        }
        ImGui::End();

        
        if (terrainChanged)
        {
            m_terrain.regenerate(m_generator, m_params);
        }
    }
}