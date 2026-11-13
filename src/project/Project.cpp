#include "project/Project.h"
#include <glad/glad.h>
#include <imgui.h>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <cmath>

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
        if (m_initialized) return;

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

        m_terrainTexture = gl::GLTexture2D::fromFile(
            "res/textures/terrain-test.png", true, "terrain.texture");

        m_terrain.initialize(m_generator, m_params);

        m_modelShader = gl::GLShaderProgram::fromFiles(
            "res/shaders/model.vert",
            "res/shaders/model.frag",
            "model.program");

        m_castle.loadFromFile("res/models/castle-tower/castle-tower.obj");
        

        m_terrainNode.parent = &m_worldNode;
        m_castleNode.parent = &m_terrainNode;

        m_castleNode.position.y = m_generator.height(0.0f, 0.0f, m_params);
        setupCastleParts();

        m_initialized = true;
        

        m_initialized = true;
    }
    void Project::setupCastleParts()
    {
        m_bodyNode.parent = &m_castleNode;
        m_mastNode.parent = &m_bodyNode;
        m_flagNode.parent = &m_mastNode;
        m_shuttersNode.parent = &m_bodyNode;

        // Punkty obrotu w ukladzie wspolrzednych modelu.
        m_mastNode.pivot = glm::vec3(0.0f, 14.55f, -0.235f);
        m_flagNode.pivot = glm::vec3(0.02f, 16.10f, -0.235f);
        m_shuttersNode.pivot = glm::vec3(0.0f, 6.375f, 2.49f);

        m_mastNode.position = m_mastNode.pivot;
        m_flagNode.position = m_flagNode.pivot - m_mastNode.pivot;
        m_shuttersNode.position = m_shuttersNode.pivot;

        m_bodyNode.parts.clear();
        m_mastNode.parts.clear();
        m_flagNode.parts.clear();
        m_shuttersNode.parts.clear();

        for (unsigned int i = 0; i < m_castle.partCount(); ++i)
        {
            const std::string& material = m_castle.partMaterial(i);

            if (material == "castle-tower_gold_x")
                m_mastNode.parts.push_back(i);
            else if (material == "castle-tower_pennant_x")
                m_flagNode.parts.push_back(i);
            else if (material == "castle-tower_shutter_x")
                m_shuttersNode.parts.push_back(i);
            else
                m_bodyNode.parts.push_back(i);
        }

        if (m_bodyNode.parts.empty() || m_mastNode.parts.empty() ||
            m_flagNode.parts.empty() || m_shuttersNode.parts.empty())
        {
            throw std::runtime_error(
                "Castle parts not found. Check material names.");
        }
    }

    void Project::update(const framework::FrameContext& frame)
    {
        ensureInitialized();

        m_camera.update(
            frame.window,
            static_cast<float>(frame.deltaSeconds),
            frame.uiWantsMouse,
            frame.uiWantsKeyboard);

        if (!m_animationPaused)
        {
            m_animationTime +=
                static_cast<float>(frame.deltaSeconds) * m_animationSpeed;
        }

        m_flagNode.animationRotationY =
            15.0f * std::sin(2.0f * m_animationTime);
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
      
        glEnable(GL_FRAMEBUFFER_SRGB);
        

        glClearColor(0.55f, 0.68f, 0.82f, 1.0f);
                
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (m_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

       
        const float aspect =
            static_cast<float>(frame.framebufferWidth) /
            static_cast<float>(frame.framebufferHeight);

       
        const glm::mat4 model = m_terrainNode.worldMatrix();

      
        const glm::mat4 view = m_camera.viewMatrix();

        const glm::mat4 projection =
            m_camera.projectionMatrix(aspect);

        
        m_shader.use();      
        m_shader.setMat4("uModel", model);
        m_shader.setMat4("uView", view);
        m_shader.setMat4("uProjection", projection);        
        m_terrainTexture.bind(0);
        m_terrain.draw();
       
        m_modelShader.use();
        m_modelShader.setMat4("uView", view);
        m_modelShader.setMat4("uProjection", projection);

        SceneNode* castleParts[] = {
            &m_bodyNode,
            &m_mastNode,
            &m_flagNode,
            &m_shuttersNode
        };

        for (const SceneNode* node : castleParts)
        {
            const glm::mat4 partModel =
                glm::translate(node->worldMatrix(), -node->pivot);

            m_castle.drawParts(m_modelShader, partModel, node->parts);
        }
        glDisable(GL_FRAMEBUFFER_SRGB);
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
            ImGui::SeparatorText("Scene");

            SceneNode* nodes[] = {
    &m_worldNode,
    &m_terrainNode,
    &m_castleNode,
    &m_bodyNode,
    &m_mastNode,
    &m_flagNode,
    &m_shuttersNode
            };

            const char* names[] = {
                m_worldNode.name.c_str(),
                m_terrainNode.name.c_str(),
                m_castleNode.name.c_str(),
                m_bodyNode.name.c_str(),
                m_mastNode.name.c_str(),
                m_flagNode.name.c_str(),
                m_shuttersNode.name.c_str()
            };

            ImGui::Combo("Object", &m_selectedNode, names, 7);

            SceneNode& node = *nodes[m_selectedNode];

            ImGui::DragFloat3("Position", &node.position.x, 0.1f);
            ImGui::DragFloat3("Rotation (deg)", &node.rotation.x, 1.0f);
            ImGui::DragFloat3("Scale", &node.scale.x, 0.01f,
                0.01f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

            const glm::vec3 globalPosition = glm::vec3(node.worldMatrix()[3]);

            ImGui::Text("Parent: %s", node.parent ? node.parent->name.c_str() : "none");
            ImGui::Text("Local: %.2f, %.2f, %.2f",
                node.position.x, node.position.y, node.position.z);
            ImGui::Text("Global: %.2f, %.2f, %.2f",
                globalPosition.x, globalPosition.y, globalPosition.z);
            ImGui::Text("Seed: %u", m_generator.seed());
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("sRGB: framebuffer");
            ImGui::SeparatorText("Animation");

            ImGui::Checkbox("Pause animation", &m_animationPaused);

            ImGui::SliderFloat(
                "Animation speed",
                &m_animationSpeed,
                0.1f,
                3.0f,
                "%.1fx",
                ImGuiSliderFlags_AlwaysClamp);

            ImGui::Text(
                "Flag Y (total): %.1f deg",
                m_flagNode.rotation.y + m_flagNode.animationRotationY);

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