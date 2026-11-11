#pragma once

// GLFW non deve includere autonomamente gl.h:
// le funzioni OpenGL vengono caricate da GLAD.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// ============================================================================
// Input
// ----------------------------------------------------------------------------
// Callback essenziali per il progetto Survivors Arena.
// ============================================================================

namespace Input
{
    extern double mouseX;
    extern double mouseY;

    // Restituisce true una sola volta quando premo F1 per aprire/chiudere il menu debug
    bool consumeDebugMenuToggle();

    void keyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods
    );

    void cursorPositionCallback(
        GLFWwindow* window,
        double xpos,
        double ypos
    );

    void framebufferSizeCallback(
        GLFWwindow* window,
        int width,
        int height
    );
}

