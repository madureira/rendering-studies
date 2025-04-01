#pragma once

#include <RenderingStudies/App.h>
#include <RenderingStudies/Types.h>

#include "../../Model/Model.h"
#include "../../Shader/Shader.h"
#include "../../Window/Window.h"
#include "../Camera.h"
#include "../Grid.h"

class BlenderModel final : public App
{
private:
    Window *m_Window;
    Shader *m_Shader;
    Camera *m_Camera;
    Grid *m_Grid;
    Model *m_Model;

public:
    BlenderModel(Window *window);
    ~BlenderModel();

    virtual void Update(float32 deltaTime) override;
};
