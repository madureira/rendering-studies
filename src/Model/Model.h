#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <RenderingStudies/Types.h>

struct aiNode;
struct aiScene;
struct aiMesh;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Mesh
{
    uint32 VAO;
    uint32 VBO;
    uint32 EBO;
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
};

class Model
{
private:
    std::vector<Mesh> m_Meshes;

public:
    Model(const std::string &path);
    void Draw() const;

private:
    void LoadModel(const std::string &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh);
    void SetupMesh(Mesh *mesh) const;
};
