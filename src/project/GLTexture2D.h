#ifndef OPENGLGP_PROJECT_GLTEXTURE2D_H
#define OPENGLGP_PROJECT_GLTEXTURE2D_H

#include <glad/glad.h>
#include <stb_image.h>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace openglgp::project::gl
{
    class GLTexture2D
    {
    public:
        GLTexture2D() = default;
        ~GLTexture2D() { if (m_id) glDeleteTextures(1, &m_id); }
        GLTexture2D(const GLTexture2D&) = delete;
        GLTexture2D& operator=(const GLTexture2D&) = delete;
        GLTexture2D(GLTexture2D&& o) noexcept : m_id(o.m_id) { o.m_id = 0; }
        GLTexture2D& operator=(GLTexture2D&& o) noexcept {
            if (this != &o) { if (m_id) glDeleteTextures(1, &m_id); m_id = o.m_id; o.m_id = 0; }
            return *this;
        }

        static GLTexture2D fromFile(const std::string& path, bool srgb, const char* label)
        {
            int width = 0, height = 0;
            unsigned char* data = stbi_load(path.c_str(), &width, &height, nullptr, 4);

            if (!data)
            {
                const char* reason = stbi_failure_reason();
                throw std::runtime_error("Cannot load texture: " + path + ": " +
                    (reason ? reason : "unknown error"));
            }

            GLTexture2D texture;
            glCreateTextures(GL_TEXTURE_2D, 1, &texture.m_id);

            if (!texture.m_id)
            {
                stbi_image_free(data);
                throw std::runtime_error("Cannot create texture: " + path);
            }

            int levels = 1;
            for (int size = std::max(width, height); size > 1; size /= 2) ++levels;

            GLenum format = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
            glTextureStorage2D(texture.m_id, levels, format, width, height);
            glTextureSubImage2D(texture.m_id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);

            glGenerateTextureMipmap(texture.m_id);
            glTextureParameteri(texture.m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(texture.m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(texture.m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(texture.m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

            if (label) glObjectLabel(GL_TEXTURE, texture.m_id, -1, label);
            return texture;
        }

        void bind(GLuint unit) const { glBindTextureUnit(unit, m_id); }

    private:
        GLuint m_id = 0;
    };
}

#endif