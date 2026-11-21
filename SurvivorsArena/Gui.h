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

    //apre o chiude il menu debug
    void toggleDebugMenu()
    {
        m_showDebugMenu = !m_showDebugMenu;
    }

    bool wireframe() const
    {
        return m_wireframe;
    }

    // parametri regolabili dal menu f1
    float playerSpeed() const { return m_playerSpeed; }
    float bulletSpeed() const { return m_bulletSpeed; }
    float fireCooldown() const { return m_fireCooldown; }
    int maxProjectiles() const { return m_maxProjectiles; }
    int batCount() const { return m_batCount; }
    float batSpeed() const { return m_batSpeed; }
    int ghostCount() const { return m_ghostCount; }
    float ghostSpeed() const { return m_ghostSpeed; }
    void setActiveProjectiles(int n) { m_activeProjectiles = n; }
    void setLives(int lives) { m_lives = lives; }
    void setScore(int score) { m_score = score; }

    bool gameStarted() const { return m_gameStarted; }
    bool resetRequested();
    bool quitRequested();

private:
    bool m_initialized = false;
    bool m_showDebugMenu = false; //parte chiuso
    bool m_wireframe = false;
    bool m_gameStarted = false;   // all'inizio mostra la schermata start

    float m_playerSpeed = 65.0f;
    float m_bulletSpeed = 140.0f;
    float m_fireCooldown = 0.18f;
    int m_maxProjectiles = 15;
    int m_batCount = 6;
    float m_batSpeed = 24.0f;
    int m_ghostCount = 3;
    float m_ghostSpeed = 13.0f;
    int m_activeProjectiles = 0;
    int m_lives = 5;
    int m_score = 0;
    bool m_resetRequested = false;
    bool m_quitRequested = false;
};

