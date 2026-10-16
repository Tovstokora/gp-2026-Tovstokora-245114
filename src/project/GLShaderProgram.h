#ifndef OPENGLGP_PROJECT_GLSHADERPROGRAM_H
#define OPENGLGP_PROJECT_GLSHADERPROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

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
            GLuint v = compileFromFile(GL_VERTEX_SHADER, vp);
            GLuint f = compileFromFile(GL_FRAGMENT_SHADER, fp);
            GLuint id = glCreateProgram();
            glAttachShader(id, v); glAttachShader(id, f);
            glLinkProgram(id);
            glDeleteShader(v); glDeleteShader(f);
            GLint ok = 0; glGetProgramiv(id, GL_LINK_STATUS, &ok);
            if (!ok) {
                char log[4096]; glGetProgramInfoLog(id, sizeof(log), nullptr, log);
                glDeleteProgram(id);
                throw std::runtime_error(std::string("Link failed: ") + log);
            }
            GLShaderProgram p; p.m_id = id;
            if (label) glObjectLabel(GL_PROGRAM, id, -1, label);
            return p;
        }
        void use() const { glUseProgram(m_id); }
        GLuint id() const { return m_id; }
        void setMat4(const char* n, const glm::mat4& m) const {
            glUniformMatrix4fv(glGetUniformLocation(m_id, n), 1, GL_FALSE, glm::value_ptr(m));
        }
        void setVec3(const char* n, const glm::vec3& v) const {
            glUniform3fv(glGetUniformLocation(m_id, n), 1, glm::value_ptr(v));
        }
    private:
        static GLuint compileFromFile(GLenum stage, const std::string& path) {
            std::ifstream in(path);
            if (!in) throw std::runtime_error("Cannot open shader: " + path);
            std::stringstream ss; ss << in.rdbuf();
            std::string src = ss.str();
            const char* p = src.c_str();
            GLuint s = glCreateShader(stage);
            glShaderSource(s, 1, &p, nullptr);
            glCompileShader(s);
            GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
            if (!ok) {
                char log[4096]; glGetShaderInfoLog(s, sizeof(log), nullptr, log);
                glDeleteShader(s);
                throw std::runtime_error("Compile failed (" + path + "): " + log);
            }
            return s;
        }
        GLuint m_id = 0;
    };

}
#endif