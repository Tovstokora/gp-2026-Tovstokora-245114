#ifndef OPENGLGP_PROJECT_GLSHADERPROGRAM_H
#define OPENGLGP_PROJECT_GLSHADERPROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace openglgp::project::gl
{

    class GLShaderProgram
    {
    public:
        GLShaderProgram() = default;
        ~GLShaderProgram() { if (m_id) glDeleteProgram(m_id); }
        GLShaderProgram(const GLShaderProgram&) = delete;
        GLShaderProgram& operator=(const GLShaderProgram&) = delete;
        GLShaderProgram(GLShaderProgram&& o) noexcept : m_id(o.m_id) { o.m_id = 0; }
        GLShaderProgram& operator=(GLShaderProgram&& o) noexcept {
            if (this != &o) { if (m_id) glDeleteProgram(m_id); m_id = o.m_id; o.m_id = 0; }
            return *this;
        }
        static GLShaderProgram fromFiles(const std::string& vp, const std::string& fp, const char* label = nullptr) {
            auto v = compileFromFile(GL_VERTEX_SHADER, vp);
            auto f = compileFromFile(GL_FRAGMENT_SHADER, fp);

            GLShaderProgram p;
            p.m_id = glCreateProgram();
            if (p.m_id == 0)
                throw std::runtime_error("glCreateProgram failed.");

            glAttachShader(p.m_id, v.id);
            glAttachShader(p.m_id, f.id);
            glLinkProgram(p.m_id);
            GLint ok = 0; glGetProgramiv(p.m_id, GL_LINK_STATUS, &ok);
            if (!ok) {
                char log[4096]{};
                glGetProgramInfoLog(p.m_id, sizeof(log), nullptr, log);
                throw std::runtime_error(std::string("Link failed: ") + log);
            }

            glDetachShader(p.m_id, v.id);
            glDetachShader(p.m_id, f.id);
            if (label) glObjectLabel(GL_PROGRAM, p.m_id, -1, label);
            return p;
        }
        void use() const { glUseProgram(m_id); }
        GLuint id() const { return m_id; }
        void setMat4(const char* n, const glm::mat4& m) const {
            glUniformMatrix4fv(glGetUniformLocation(m_id, n), 1, GL_FALSE, glm::value_ptr(m));
        }
    private:
        struct ShaderHandle {
            GLuint id;
            explicit ShaderHandle(GLuint value) noexcept : id(value) {}
            ~ShaderHandle() { if (id) glDeleteShader(id); }
            ShaderHandle(const ShaderHandle&) = delete;
            ShaderHandle& operator=(const ShaderHandle&) = delete;
            ShaderHandle(ShaderHandle&& other) noexcept : id(std::exchange(other.id, 0)) {}
            ShaderHandle& operator=(ShaderHandle&&) = delete;
        };

        static ShaderHandle compileFromFile(GLenum stage, const std::string& path) {
            std::ifstream in(path);
            if (!in) throw std::runtime_error("Cannot open shader: " + path);
            std::stringstream ss; ss << in.rdbuf();
            std::string src = ss.str();
            const char* p = src.c_str();
            ShaderHandle s(glCreateShader(stage));
            if (s.id == 0)
                throw std::runtime_error("glCreateShader failed for: " + path);
            glShaderSource(s.id, 1, &p, nullptr);
            glCompileShader(s.id);
            GLint ok = 0; glGetShaderiv(s.id, GL_COMPILE_STATUS, &ok);
            if (!ok) {
                char log[4096]{};
                glGetShaderInfoLog(s.id, sizeof(log), nullptr, log);
                throw std::runtime_error("Compile failed (" + path + "): " + log);
            }
            return s;
        }
        GLuint m_id = 0;
    };

}
#endif
