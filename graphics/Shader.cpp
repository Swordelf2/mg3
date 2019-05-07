#include <GL/glew.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

Shader *Shader::m_curUsed = nullptr;

Shader::Shader(const std::string &vertPath, const std::string &fragPath)
{
    Load(vertPath, fragPath);
}

Shader::Shader(Shader &&other) :
    m_id(other.m_id),
    m_uniLocation(other.m_uniLocation)
{
    other.m_id = 0;
}

Shader::~Shader()
{
    if (m_id) {
        glDeleteProgram(m_id);
    }
}

void Shader::Use()
{
    if (!IsUsed()) {
        glUseProgram(m_id);
        m_curUsed = this;
    }
}

void Shader::Unuse()
{
    if (IsUsed()) {
        glUseProgram(0);
        m_curUsed = nullptr;
    }
}

bool Shader::IsUsed() const
{
    return m_curUsed == this;
}

GLint Shader::GetUniLocation(const std::string &uniName)
{
    if (auto it = m_uniLocation.find(uniName); it != m_uniLocation.end()) {
        return it->second;
    } else {
        GLint location = glGetUniformLocation(m_id, uniName.c_str());
        if (location == -1) {
            std::cerr << "Could not find uniform: " << uniName << std::endl;
        } else {
            m_uniLocation.insert({uniName, location});
        }
        return location;
    }
}

void Shader::SetUniform(const std::string &uniName, GLint val)
{
    Use();
    glUniform1i(GetUniLocation(uniName), val);
}

void Shader::SetUniform(const std::string &uniName, const glm::vec3 &val)
{
    Use();
    glUniform3fv(GetUniLocation(uniName), 1, glm::value_ptr(val));
}

void Shader::SetUniform(const std::string &uniName, const glm::vec4 &val)
{
    Use();
    glUniform4fv(GetUniLocation(uniName), 1, glm::value_ptr(val));
}

void Shader::SetUniform(const std::string &uniName, const glm::mat4 &val)
{
    Use();
    glUniformMatrix4fv(GetUniLocation(uniName), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::Load(const std::string &vertPath, const std::string &fragPath)
{
    std::string vCode, fCode;
    std::ifstream vFile, fFile;
    try {
        vFile.open(vertPath);
        fFile.open(fragPath);
        std::stringstream vStream, fStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        vFile.close();
        fFile.close();
        vCode = vStream.str();
        fCode = fStream.str();
    } catch (std::ifstream::failure &e) {
        std::cerr << "Error while opening shader source file: " <<
                std::endl;
        std::cerr << e.what() << std::endl;
    }

    GLuint vId, fId;
    vId = CompileShader(vCode, GL_VERTEX_SHADER);
    fId = CompileShader(fCode, GL_FRAGMENT_SHADER);

    m_id = glCreateProgram();

    glAttachShader(m_id, vId);
    glAttachShader(m_id, fId);
    glLinkProgram(m_id);

    GLint result;
    glGetProgramiv(m_id, GL_LINK_STATUS, &result);
    if (result != GL_TRUE) {
        char message[1024];
        glGetProgramInfoLog(m_id, sizeof(message), nullptr, message);
        std::cerr << "Failed to link shader: " << std::endl;
        std::cerr << vertPath << std::endl << fragPath << std::endl;
        std::cerr << message << std::endl;
    }

    glDeleteShader(vId);
    glDeleteShader(fId);
}

GLuint Shader::CompileShader(const std::string& source, GLenum type)
{
    unsigned id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        // Get the message
        char message[1024];
        glGetShaderInfoLog(id, sizeof(message), nullptr, message);
        std::cerr << "Failed to compile shader!" <<
                (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
                std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}
