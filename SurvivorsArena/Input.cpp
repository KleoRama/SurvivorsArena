#include <glad/glad.h>
#include "Input.h"


namespace Input
{
    double mouseX = 0.0;
    double mouseY = 0.0;
    namespace
    {
        // Diventa true quando premo F1 e si resetta appena il main lo legge
        bool debugMenuToggleRequested = false;
    }

    bool consumeDebugMenuToggle()
    {
        const bool requested = debugMenuToggleRequested;
        debugMenuToggleRequested = false;
        return requested;
    }


    void keyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods)
    {
        if (
            key == GLFW_KEY_ESCAPE
            && action == GLFW_PRESS
            )
        {
            glfwSetWindowShouldClose(
                window,
                GLFW_TRUE
            );
        }

        // Segnala la pressione di F1 (solo al primo tocco, non se tenuto premuto)
        if (
            key == GLFW_KEY_F1
            && action == GLFW_PRESS
            )
        {
            debugMenuToggleRequested = true;
        }
    }


    void cursorPositionCallback(
        GLFWwindow* window,
        double xpos,
        double ypos)
    {
        int width;
        int height;

        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        mouseX = xpos;

        // GLFW misura y dall'alto verso il basso.
        // La convertiamo nel riferimento usato nella scena.
        mouseY = height - ypos;
    }


    void framebufferSizeCallback(
        GLFWwindow* window,
        int width,
        int height)
    {
        glViewport(
            0,
            0,
            width,
            height
        );
    }
}

