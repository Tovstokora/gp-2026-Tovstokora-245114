#ifndef OPENGLGP_PROJECT_PROJECT_H
#define OPENGLGP_PROJECT_PROJECT_H


#include "framework/FrameContext.h"
#include "project/Camera.h"
#include "project/Terrain.h"
#include "project/TerrainGenerator.h"
#include "project/GLShaderProgram.h"

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
        bool m_initialized = false;
        TerrainGenerator m_generator{ 245114u };
        TerrainParams m_params;
        Terrain m_terrain;
        Camera m_camera;
        gl::GLShaderProgram m_shader;

        bool m_wireframe = false;
    };
}

#endif