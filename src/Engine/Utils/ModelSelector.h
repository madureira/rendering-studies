#pragma once

#include "../Model/Model.h"

#define TOTAL_MODELS 8

class ModelSelector final
{
private:
    Model* m_Model = nullptr;
    int32 m_CurrentMesh = 0;
    int32 m_LoadedMeshIndex = -1;
    static const char* const s_MeshOptions[TOTAL_MODELS];
    static const char* const s_MeshPaths[TOTAL_MODELS];

public:
    ModelSelector(const int32 modelIndex = 0);
    ~ModelSelector();

    void Render();

    const Model& GetSelectedModel() const;

private:
    void LoadCurrentSelectedModel();
};
