#include "Gui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void Gui::initialize(GLFWwindow* window)
{
    if (m_initialized) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    m_initialized = true;
}

void Gui::beginFrame()
{
    if (!m_initialized) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::draw()
{
    ImGui::SetNextWindowPos(ImVec2(12, 12), ImGuiCond_FirstUseEver);
    ImGui::Begin("Survivors Arena - Controlli");

    ImGui::Text("Colore dello sfondo");
    ImGui::ColorEdit3("Background", &m_clearColor.r);

    ImGui::Separator();

    ImGui::Text("Modalita' di visualizzazione");
    ImGui::Checkbox("Wireframe", &m_wireframe);

    ImGui::End();
}

void Gui::render()
{
    if (!m_initialized) return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::shutdown()
{
    if (!m_initialized) return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_initialized = false;
}

