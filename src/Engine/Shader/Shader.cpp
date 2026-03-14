#include "Shader.h"

#include <RenderingStudies/GL.h>

#include "../FileManager/FileManager.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    m_VertexCode = FileManager::ReadText(vertexPath);
    m_FragmentCode = FileManager::ReadText(fragmentPath);
    Compile();
    Link();
}

Shader::Shader(const std::string& vertexPath, const std::string& tessControlPath, const std::string& tessEvalPath, const std::string& fragmentPath)
{
    m_VertexCode = FileManager::ReadText(vertexPath);
    m_TessControlCode = FileManager::ReadText(tessControlPath);
    m_TessEvalCode = FileManager::ReadText(tessEvalPath);
    m_FragmentCode = FileManager::ReadText(fragmentPath);
    Compile();
    Link();
}

Shader::~Shader()
{
    if (m_ID != 0)
    {
        GL(glDeleteProgram(m_ID));
    }
}

void Shader::Bind() const
{
    if (m_ID == 0)
    {
        LOG_ERROR("Shader: Bind() called but program is 0 (shader failed to link or was deleted).");
        return;
    }
    GL(glUseProgram(m_ID));
}

void Shader::Unbind() const
{
    GL(glUseProgram(0));
}

uint32 Shader::GetProgram() const
{
    return m_ID;
}

void Shader::SetBool(const std::string& name, bool value) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform1i(loc, (int32)value));
    }
}

void Shader::SetInt(const std::string& name, int32 value) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform1i(loc, value));
    }
}

void Shader::SetFloat(const std::string& name, float32 value) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform1f(loc, value));
    }
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform2fv(loc, 1, &value[0]));
    }
}
void Shader::SetVec2(const std::string& name, float32 x, float32 y) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform2f(loc, x, y));
    }
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform3fv(loc, 1, &value[0]));
    }
}
void Shader::SetVec3(const std::string& name, float32 x, float32 y, float32 z) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform3f(loc, x, y, z));
    }
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform4fv(loc, 1, &value[0]));
    }
}
void Shader::SetVec4(const std::string& name, float32 x, float32 y, float32 z, float32 w) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniform4f(loc, x, y, z, w));
    }
}

void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniformMatrix2fv(loc, 1, GL_FALSE, &mat[0][0]));
    }
}

void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]));
    }
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    int32 loc = GetUniformLocation(name);
    if (loc >= 0)
    {
        GL(glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]));
    }
}

void Shader::Compile()
{
    const char* vsCode = m_VertexCode.c_str();
    m_VertexId = GLR(glCreateShader(GL_VERTEX_SHADER));
    GL(glShaderSource(m_VertexId, 1, &vsCode, NULL));
    GL(glCompileShader(m_VertexId));
    CheckCompileError(m_VertexId, "Vertex Shader");

    if (!m_TessControlCode.empty() && !m_TessEvalCode.empty())
    {
        const char* tcsCode = m_TessControlCode.c_str();
        m_TessControlId = GLR(glCreateShader(GL_TESS_CONTROL_SHADER));
        GL(glShaderSource(m_TessControlId, 1, &tcsCode, NULL));
        GL(glCompileShader(m_TessControlId));
        CheckCompileError(m_TessControlId, "Tessellation Control Shader");

        const char* tesCode = m_TessEvalCode.c_str();
        m_TessEvalId = GLR(glCreateShader(GL_TESS_EVALUATION_SHADER));
        GL(glShaderSource(m_TessEvalId, 1, &tesCode, NULL));
        GL(glCompileShader(m_TessEvalId));
        CheckCompileError(m_TessEvalId, "Tessellation Evaluation Shader");
    }

    const char* fsCode = m_FragmentCode.c_str();
    m_FragmentId = GLR(glCreateShader(GL_FRAGMENT_SHADER));
    GL(glShaderSource(m_FragmentId, 1, &fsCode, NULL));
    GL(glCompileShader(m_FragmentId));
    CheckCompileError(m_FragmentId, "Fragment Shader");
}

void Shader::Link()
{
    m_ID = GLR(glCreateProgram());
    GL(glAttachShader(m_ID, m_VertexId));
    if (!m_TessControlCode.empty() && !m_TessEvalCode.empty())
    {
        GL(glAttachShader(m_ID, m_TessControlId));
        GL(glAttachShader(m_ID, m_TessEvalId));
    }
    GL(glAttachShader(m_ID, m_FragmentId));
    GL(glLinkProgram(m_ID));
    CheckLinkingError();
    GL(glDeleteShader(m_VertexId));
    if (!m_TessControlCode.empty() && !m_TessEvalCode.empty())
    {
        GL(glDeleteShader(m_TessControlId));
        GL(glDeleteShader(m_TessEvalId));
    }
    GL(glDeleteShader(m_FragmentId));
}

void Shader::CheckCompileError(uint32 shader, const std::string type) const
{
    int32 success;
    char infoLog[1024];
    GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success)
    {
        GL(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
        LOG_ERROR("Shader: Error compiling {}:\n{}", type, infoLog);
    }
}

void Shader::CheckLinkingError()
{
    int32 success;
    char infoLog[1024];
    GL(glGetProgramiv(m_ID, GL_LINK_STATUS, &success));
    if (!success)
    {
        GL(glGetProgramInfoLog(m_ID, 1024, NULL, infoLog));
        LOG_ERROR("Shader: Error linking shader program:\n{}", infoLog);
        GL(glDeleteProgram(m_ID));
        m_ID = 0;
    }
}

int32 Shader::GetUniformLocation(const std::string& name) const
{
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
    {
        return it->second;
    }

    int32 location = GLR(glGetUniformLocation(m_ID, name.c_str()));

    if (location == -1)
    {
        LOG_ERROR("Shader: Uniform {} not found", name);
    }

    m_UniformLocationCache[name] = location;

    return location;
}
