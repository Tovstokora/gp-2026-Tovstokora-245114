#ifndef OPENGLGP_PROJECT_GLBUFFER_H
#define OPENGLGP_PROJECT_GLBUFFER_H

#include <glad/glad.h>
#include <cstddef>

namespace openglgp::project::gl
{

    class GLBuffer
    {
    public:
        GLBuffer() = default;
        GLBuffer(const void* data, std::size_t sizeBytes, const char* label)
        {
            glCreateBuffers(1, &m_id);
            glNamedBufferStorage(m_id, sizeBytes, data, 0);
            if (label) glObjectLabel(GL_BUFFER, m_id, -1, label);
        }
        ~GLBuffer() { if (m_id) glDeleteBuffers(1, &m_id); }
        GLBuffer(const GLBuffer&) = delete;
        GLBuffer& operator=(const GLBuffer&) = delete;
        GLBuffer(GLBuffer&& o) noexcept : m_id(o.m_id) { o.m_id = 0; }
        GLBuffer& operator=(GLBuffer&& o) noexcept {
            if (this != &o) { if (m_id) glDeleteBuffers(1, &m_id); m_id = o.m_id; o.m_id = 0; }
            return *this;
        }
        GLuint id() const { return m_id; }
    private:
        GLuint m_id = 0;
    };

}
#endif
