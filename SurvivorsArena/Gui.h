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

    const glm::vec4& clearColor() const
    {
        return m_clearColor;
    }

    bool wireframe() const
    {
        return m_wireframe;
    }

private:
    glm::vec4 m_clearColor{0.05f, 0.08f, 0.14f, 1.0f};
    bool m_initialized = false;
    bool m_wireframe = false;
};

