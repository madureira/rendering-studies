#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

#include <RenderingStudies/Types.h>

class Shader
{
private:
    uint32 m_ShaderId;
    uint32 m_VertexId;
    uint32 m_FragmentId;
    std::string m_VertexCode;
    std::string m_FragmentCode;

public:
    Shader(const std::string &vertexCode, const std::string &fragmentCode);
    ~Shader();

    void Use();
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int32 value) const;
    void SetFloat(const std::string &name, float32 value) const;
    void SetVec2(const std::string &name, const glm::vec2 &value) const;
    void SetVec2(const std::string &name, float32 x, float32 y) const;
    void SetVec3(const std::string &name, const glm::vec3 &value) const;
    void SetVec3(const std::string &name, float32 x, float32 y, float32 z) const;
    void SetVec4(const std::string &name, const glm::vec4 &value) const;
    void SetVec4(const std::string &name, float32 x, float32 y, float32 z, float32 w) const;
    void SetMat2(const std::string &name, const glm::mat2 &mat) const;
    void SetMat3(const std::string &name, const glm::mat3 &mat) const;
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void Compile();
    void Link();
    void CheckCompileError(uint32 shader, const std::string type);
    void CheckLinkingError();
};
