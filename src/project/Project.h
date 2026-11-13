#ifndef OPENGLGP_PROJECT_PROJECT_H
#define OPENGLGP_PROJECT_PROJECT_H

#include "framework/FrameContext.h"
#include "project/Camera.h"
#include "project/Terrain.h"
#include "project/TerrainGenerator.h"
#include "project/GLShaderProgram.h"
#include "project/GLTexture2D.h"
#include "project/Model.h"
#include "project/SceneNode.h"

namespace openglgp::project
{
    class Project
    {
    public:
        void update(const framework::FrameContext& frame);
        void render(const framework::FrameContext& frame);
        void drawGui();

    private:
        void ensureInitialized();
        void setupCastleParts();

        bool m_initialized = false;
        TerrainGenerator m_generator{ 245114u };
        TerrainParams m_params;
        Terrain m_terrain;

        Model m_castle;
        gl::GLShaderProgram m_modelShader;

        SceneNode m_worldNode{ "World" };
        SceneNode m_terrainNode{ "Terrain" };
        SceneNode m_castleNode{ "Castle" };
        SceneNode m_bodyNode{ "Body" };
        SceneNode m_mastNode{ "Mast" };
        SceneNode m_flagNode{ "Flag" };
        SceneNode m_shuttersNode{ "Shutters" };
        int m_selectedNode = 2;

        Camera m_camera;
        gl::GLShaderProgram m_shader;
        gl::GLTexture2D m_terrainTexture;

        bool m_wireframe = false;
        bool m_animationPaused = false;
        float m_animationTime = 0.0f;
        float m_animationSpeed = 1.0f;
    };
}

#endif