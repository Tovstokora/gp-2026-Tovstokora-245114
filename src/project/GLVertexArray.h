#ifndef OPENGLGP_PROJECT_GLVERTEXARRAY_H
#define OPENGLGP_PROJECT_GLVERTEXARRAY_H

#include <glad/glad.h>

namespace openglgp::project::gl
{

    class GLVertexArray
    {
    public:
        GLVertexArray() { glCreateVertexArrays(1, &m_id); }
        ~GLVertexArray() { if (m_id) glDeleteVertexArrays(1, &m_id); }
        GLVertexArray(const GLVertexArray&) = delete;
        GLVertexArray& operator=(const GLVertexArray&) = delete;
        GLVertexArray(GLVertexArray&& o) noexcept : m_id(o.m_id) { o.m_id = 0; }
        GLVertexArray& operator=(GLVertexArray&& o) noexcept {
            if (this != &o) { if (m_id) glDeleteVertexArrays(1, &m_id); m_id = o.m_id; o.m_id = 0; }
            return *this;
        }
        void setLabel(const char* label) { glObjectLabel(GL_VERTEX_ARRAY, m_id, -1, label); }
        void bindVertexBuffer(GLuint b, GLuint buf, GLintptr off, GLsizei stride) {
            glVertexArrayVertexBuffer(m_id, b, buf, off, stride);
        }
        void bindElementBuffer(GLuint buf) { glVertexArrayElementBuffer(m_id, buf); }
        void attribFormat(GLuint i, GLint s, GLenum t, GLboolean n, GLuint r) {
            glVertexArrayAttribFormat(m_id, i, s, t, n, r);
        }
        void attribBinding(GLuint i, GLuint b) { glVertexArrayAttribBinding(m_id, i, b); }
        void enableAttrib(GLuint i) { glEnableVertexArrayAttrib(m_id, i); }
        void bind()   const { glBindVertexArray(m_id); }
        void unbind() const { glBindVertexArray(0); }
        GLuint id() const { return m_id; }
    private:
        GLuint m_id = 0;
    };

}
#endif