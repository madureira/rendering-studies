#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

#include <rendering_studies/types.h>

class Shader
{
private:
    uint32 m_id;
    uint32 m_vertexId;
    uint32 m_fragmentId;
    std::string m_vertexCode;
    std::string m_fragmentCode;

public:
    Shader(const std::string &vertexCode, const std::string &fragmentCode);
    ~Shader();

    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int32 value) const;
    void setFloat(const std::string &name, float32 value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float32 x, float32 y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float32 x, float32 y, float32 z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float32 x, float32 y, float32 z, float32 w) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void compile();
    void link();
    void checkCompileError(uint32 shader, const std::string type);
    void checkLinkingError();
};
