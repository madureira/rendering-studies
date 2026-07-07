#include "ModelSelector.h"

#include <imgui.h>

const char* const ModelSelector::s_MeshOptions[TOTAL_MODELS] = {
    "apple",
    "bunny",
    "cube",
    "cylinder",
    "dragon",
    "monkey",
    "sphere",
    "teapot"
};

const char* const ModelSelector::s_MeshPaths[TOTAL_MODELS] = {
    "assets/models/apple.fbx",
    "assets/models/bunny.obj",
    "assets/models/cube.obj",
    "assets/models/cylinder.obj",
    "assets/models/dragon.obj",
    "assets/models/monkey.obj",
    "assets/models/sphere.obj",
    "assets/models/teapot.obj"
};

ModelSelector::ModelSelector(const int32 modelIndex)
{
    m_CurrentMesh = modelIndex;
    LoadCurrentSelectedModel();
}

ModelSelector::~ModelSelector()
{
    delete m_Model;
}

void ModelSelector::Render()
{
    ImGui::TextUnformatted("Meshes");
    ImGui::SameLine();
    ImGui::Combo("##Meshes", &m_CurrentMesh, s_MeshOptions, IM_ARRAYSIZE(s_MeshOptions));

    if (m_CurrentMesh != m_LoadedMeshIndex)
    {
        LoadCurrentSelectedModel();
    }
}

const Model& ModelSelector::GetSelectedModel() const
{
    return *m_Model;
}

void ModelSelector::LoadCurrentSelectedModel()
{
    delete m_Model;
    m_Model = new Model(s_MeshPaths[m_CurrentMesh]);
    m_LoadedMeshIndex = m_CurrentMesh;
}
