#include "Shader.h"

#include <GL/glew.h>

#include "../FileManager/FileManager.h"

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
{
    m_VertexCode = FileManager::ReadText(vertexPath);
    m_FragmentCode = FileManager::ReadText(fragmentPath);
    Compile();
    Link();
}

Shader::~Shader()
{
    if (m_ID != 0)
    {
        glDeleteProgram(m_ID);
    }
}

void Shader::Bind()
{
    glUseProgram(m_ID);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

uint32 Shader::GetProgram() const
{
    return m_ID;
}

void Shader::SetBool(const std::string &name, bool value) const
{
    glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const std::string &name, int32 value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string &name, float32 value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}
void Shader::SetVec2(const std::string &name, float32 x, float32 y) const
{
    glUniform2f(GetUniformLocation(name), x, y);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}
void Shader::SetVec3(const std::string &name, float32 x, float32 y, float32 z) const
{
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}
void Shader::SetVec4(const std::string &name, float32 x, float32 y, float32 z, float32 w) const
{
    glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::Compile()
{
    const char *vsCode = m_VertexCode.c_str();
    m_VertexId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_VertexId, 1, &vsCode, NULL);
    glCompileShader(m_VertexId);
    CheckCompileError(m_VertexId, "Vertex Shader");

    const char *fsCode = m_FragmentCode.c_str();
    m_FragmentId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_FragmentId, 1, &fsCode, NULL);
    glCompileShader(m_FragmentId);
    CheckCompileError(m_FragmentId, "Fragment Shader");
}

void Shader::Link()
{
    m_ID = glCreateProgram();
    glAttachShader(m_ID, m_VertexId);
    glAttachShader(m_ID, m_FragmentId);
    glLinkProgram(m_ID);
    CheckLinkingError();
    glDeleteShader(m_VertexId);
    glDeleteShader(m_FragmentId);
}

void Shader::CheckCompileError(uint32 shader, const std::string type)
{
    int32 success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        LOG_ERROR("Shader: Error compiling {}:\n{}", type, infoLog);
    }
}

void Shader::CheckLinkingError()
{
    int32 success;
    char infoLog[1024];
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ID, 1024, NULL, infoLog);
        LOG_ERROR("Shader: Error linking shader program: {}", infoLog);
    }
}

int32 Shader::GetUniformLocation(const std::string &name) const
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    int32 location = glGetUniformLocation(m_ID, name.c_str());

    if (location == -1)
    {
        LOG_ERROR("Shader: Uniform {} not found", name);
    }

    m_UniformLocationCache[name] = location;

    return location;
}
