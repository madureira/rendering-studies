#include "Shader.h"

Shader::Shader(const std::string &vertexCode, const std::string &fragmentCode)
{
    m_VertexCode = vertexCode;
    m_FragmentCode = fragmentCode;
    Compile();
    Link();
}

Shader::~Shader()
{
    if (m_ShaderId != 0)
    {
        glDeleteProgram(m_ShaderId);
    }
}

void Shader::Use()
{
    glUseProgram(m_ShaderId);
}

void Shader::SetBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_ShaderId, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int32 value) const
{
    glUniform1i(glGetUniformLocation(m_ShaderId, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float32 value) const
{
    glUniform1f(glGetUniformLocation(m_ShaderId, name.c_str()), value);
}

void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(m_ShaderId, name.c_str()), 1, &value[0]);
}
void Shader::SetVec2(const std::string &name, float32 x, float32 y) const
{
    glUniform2f(glGetUniformLocation(m_ShaderId, name.c_str()), x, y);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(m_ShaderId, name.c_str()), 1, &value[0]);
}
void Shader::SetVec3(const std::string &name, float32 x, float32 y, float32 z) const
{
    glUniform3f(glGetUniformLocation(m_ShaderId, name.c_str()), x, y, z);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(m_ShaderId, name.c_str()), 1, &value[0]);
}
void Shader::SetVec4(const std::string &name, float32 x, float32 y, float32 z, float32 w) const
{
    glUniform4f(glGetUniformLocation(m_ShaderId, name.c_str()), x, y, z, w);
}

void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_ShaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_ShaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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
    m_ShaderId = glCreateProgram();
    glAttachShader(m_ShaderId, m_VertexId);
    glAttachShader(m_ShaderId, m_FragmentId);
    glLinkProgram(m_ShaderId);
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
    glGetProgramiv(m_ShaderId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ShaderId, 1024, NULL, infoLog);
        LOG_ERROR("Shader: Error linking shader program: {}", infoLog);
    }
}
