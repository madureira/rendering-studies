#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

#include "../../Model/Model.h"
#include "../../Shader/Shader.h"
#include "../../Window/Window.h"
#include "../Camera.h"
#include "../Grid.h"

class BlenderModel : public App
{
private:
    Window *m_Window;
    Shader *m_Shader;
    Camera *m_Camera;
    Grid *m_Grid;
    Model *m_Model;

    float32 m_DeltaTime = 0.0f;
    float32 m_LastFrame = 0.0f;
    float32 m_LastX;
    float32 m_LastY;
    bool m_FirstMouse = true;

public:
    BlenderModel(Window *window);
    ~BlenderModel();

    virtual void Update() override;
};
