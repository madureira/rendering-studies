#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <RenderingStudies/GL.h>

Model::Model(const std::string& path)
{
    LoadModel(path);
}

void Model::Draw() const
{
    for (const auto& mesh : m_Meshes)
    {
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Model::LoadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR("ERROR::ASSIMP::{}", importer.GetErrorString());
        return;
    }

    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (uint32 i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh));
    }
    for (uint32 i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* mesh)
{
    Mesh newMesh;

    for (uint32 i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        if (mesh->mTextureCoords[0]) // If the mesh contains texture coordinates
        {
            vertex.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else
        {
            vertex.TexCoords = { 0.0f, 0.0f };
        }

        newMesh.vertices.push_back(vertex);
    }

    for (uint32 i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32 j = 0; j < face.mNumIndices; j++)
        {
            newMesh.indices.push_back(face.mIndices[j]);
        }
    }

    SetupMesh(&newMesh);

    return newMesh;
}

void Model::SetupMesh(Mesh* mesh) const
{
    GL(glGenVertexArrays(1, &mesh->VAO));
    GL(glGenBuffers(1, &mesh->VBO));
    GL(glGenBuffers(1, &mesh->EBO));

    GL(glBindVertexArray(mesh->VAO));

    GL(glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW));

    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(uint32), &mesh->indices[0], GL_STATIC_DRAW));

    // Position attribute
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
    GL(glEnableVertexAttribArray(0));

    // Normal attribute
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)));
    GL(glEnableVertexAttribArray(1));

    // Texture coordinate attribute
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)));
    GL(glEnableVertexAttribArray(2));

    // Unbind objects
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
}
