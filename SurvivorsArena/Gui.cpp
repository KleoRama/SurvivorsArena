#include "Gui.h"

#include <cstdio>

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
    const ImGuiIO& io = ImGui::GetIO();
    const ImGuiWindowFlags fullScreenFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    //schermata iniziale a tutto schermo
    if (!m_gameStarted)
    {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.03f, 0.01f, 0.04f, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.10f, 0.14f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.16f, 0.20f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.32f, 0.07f, 0.10f, 1.0f));

        ImGui::Begin("##StartScreen", nullptr, fullScreenFlags);

        //titolo centrato
        ImGui::SetWindowFontScale(3.0f);
        const char* title = "SURVIVORS ARENA";
        const float titleWidth = ImGui::CalcTextSize(title).x;
        ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - titleWidth) * 0.5f, io.DisplaySize.y * 0.26f));
        ImGui::TextColored(ImVec4(0.95f, 0.25f, 0.22f, 1.0f), "%s", title);

        // sottotitolo comandi
        ImGui::SetWindowFontScale(1.3f);
        const char* sub = "WASD per muoverti  -  Mouse per mirare  -  Click Sinistro o SPAZIO per sparare";
        const float subWidth = ImGui::CalcTextSize(sub).x;
        ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - subWidth) * 0.5f, io.DisplaySize.y * 0.38f));
        ImGui::TextColored(ImVec4(0.82f, 0.80f, 0.86f, 1.0f), "%s", sub);

        //bottoni
        ImGui::SetWindowFontScale(1.5f);
        const float btnW = 260.0f;
        const float btnH = 54.0f;
        ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - btnW) * 0.5f, io.DisplaySize.y * 0.48f));
        if (ImGui::Button("Inizia gioco", ImVec2(btnW, btnH)))
        {
            m_gameStarted = true;
            m_resetRequested = true;
        }

        ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - btnW) * 0.5f, io.DisplaySize.y * 0.48f + 72.0f));
        if (ImGui::Button("Esci dal gioco", ImVec2(btnW, btnH)))
        {
            m_quitRequested = true;
        }

        ImGui::SetWindowFontScale(1.0f);
        ImGui::End();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
    }
    // schermata di game over quando finiscono le vite
    else if (m_lives <= 0)
    {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.01f, 0.02f, 0.88f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.10f, 0.14f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.16f, 0.20f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.32f, 0.07f, 0.10f, 1.0f));

        ImGui::Begin("##GameOverScreen", nullptr, fullScreenFlags);

        ImGui::SetWindowFontScale(3.0f);
        const char* overTitle = "SEI MORTO";
        const float overWidth = ImGui::CalcTextSize(overTitle).x;
        ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - overWidth) * 0.5f, io.DisplaySize.y * 0.25f));
        ImGui::TextColored(ImVec4(1.0f, 0.20f, 0.18f, 1.0f), "%s", overTitle);

        ImGui::SetWindowFontScale(1.8f);
        char scoreText[64];
        snprintf(scoreText, sizeof(scoreText), "SCORE: %d", m_score);
        const float scoreWidth = ImGui::CalcTextSize(scoreText).x;
        ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - scoreWidth) * 0.5f, io.DisplaySize.y * 0.37f));
        ImGui::TextColored(ImVec4(1.0f, 0.88f, 0.50f, 1.0f), "%s", scoreText);

        ImGui::SetWindowFontScale(1.5f);
        const float btnW = 260.0f;
        const float btnH = 54.0f;
        ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - btnW) * 0.5f, io.DisplaySize.y * 0.48f));
        if (ImGui::Button("Ricomincia", ImVec2(btnW, btnH)))
        {
            m_resetRequested = true;
        }

        ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - btnW) * 0.5f, io.DisplaySize.y * 0.48f + 72.0f));
        if (ImGui::Button("Esci dal gioco", ImVec2(btnW, btnH)))
        {
            m_quitRequested = true;
        }

        ImGui::SetWindowFontScale(1.0f);
        ImGui::End();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
    }
    //in partita mostra lo score sotto ai cuori
    else
    {
        const ImGuiWindowFlags hudFlags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings;
        ImGui::SetNextWindowPos(ImVec2(16.0f, 76.0f), ImGuiCond_Always);
        ImGui::Begin("##HUD_Score", nullptr, hudFlags);
        ImGui::SetWindowFontScale(1.8f);
        ImGui::TextColored(ImVec4(1.0f, 0.88f, 0.50f, 1.0f), "SCORE: %d", m_score);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::End();
    }

    //menu debug che si apre con f1
    if (m_showDebugMenu)
    {
        ImGui::SetNextWindowPos(ImVec2(12, 110), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(370, 340), ImGuiCond_FirstUseEver);
        ImGui::Begin("Debug e Statistiche (F1)", &m_showDebugMenu);

        ImGui::Text("Parametri Player e Balestra");
        ImGui::SliderFloat("Velocita' player", &m_playerSpeed, 20.0f, 150.0f);
        ImGui::SliderFloat("Velocita' proiettili", &m_bulletSpeed, 40.0f, 280.0f);
        ImGui::SliderFloat("Ricarica sparo (s)", &m_fireCooldown, 0.05f, 0.80f);
        ImGui::SliderInt("Max proiettili", &m_maxProjectiles, 1, 30);

        ImGui::Separator();
        ImGui::Text("Parametri Nemici (Pipistrelli e Fantasmi)");
        ImGui::SliderInt("Numero pipistrelli", &m_batCount, 0, 15);
        ImGui::SliderFloat("Velocita' pipistrelli", &m_batSpeed, 5.0f, 65.0f);
        ImGui::SliderInt("Numero fantasmi", &m_ghostCount, 0, 10);
        ImGui::SliderFloat("Velocita' fantasmi", &m_ghostSpeed, 4.0f, 40.0f);

        ImGui::Separator();
        ImGui::Text("Cuori di vita: %d / 5", m_lives);
        ImGui::Text("Score (Gemme raccolte): %d", m_score);
        ImGui::Text("Proiettili attivi: %d / %d", m_activeProjectiles, m_maxProjectiles);
        if (ImGui::Button("Ripristina scena"))
            m_resetRequested = true;

        ImGui::Separator();
        ImGui::Checkbox("Wireframe", &m_wireframe);
        ImGui::End();
    }
}

bool Gui::resetRequested()
{
    const bool result = m_resetRequested;
    m_resetRequested = false;
    return result;
}

bool Gui::quitRequested()
{
    const bool result = m_quitRequested;
    m_quitRequested = false;
    return result;
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

