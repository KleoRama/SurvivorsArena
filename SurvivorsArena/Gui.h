#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

// ============================================================================
// Gui
// Incapsula Dear ImGui e i parametri modificabili dall'utente.
// ============================================================================
class Gui
{
public:
    void initialize(GLFWwindow* window);
    void beginFrame();
    void draw();
    void render();
    void shutdown();

    // Apre o chiude il pannello debug
    void toggleDebugMenu()
    {
        m_showDebugMenu = !m_showDebugMenu;
    }

    bool wireframe() const
    {
        return m_wireframe;
    }

private:
    bool m_initialized = false;
    bool m_showDebugMenu = false; // All'avvio il menu debug parte chiuso
    bool m_wireframe = false;
};

