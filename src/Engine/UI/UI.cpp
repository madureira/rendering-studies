#include "UI.h"

#include "../Window/Window.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

UI::UI(const Window& window)
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.getNativeWindow(), true);

#ifdef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_Init("#version 300 es");
    {
        float64 dpr = EM_ASM_DOUBLE({ return window.devicePixelRatio || 1.0; });
        if (dpr < 1.0)
        {
            dpr = 1.0;
        }
        ImGuiIO& io = ImGui::GetIO();
        const float32 baseFontSize = 20.0f;
        io.Fonts->AddFontFromFileTTF("assets/fonts/roboto-regular.ttf", baseFontSize * (float)dpr);
        io.FontGlobalScale = 1.0f / (float)dpr;
        ImGui::GetStyle().WindowMinSize.x = 200.0f;
    }
#else
    ImGui_ImplOpenGL3_Init("#version 410 core");
    ImGui::GetStyle().WindowMinSize.x = 180.0f;
#endif
    ImGui::GetStyle().ScaleAllSizes(1.5f);
}

UI::~UI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::NewFrame()
{
    // Per-frame:
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UI::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
