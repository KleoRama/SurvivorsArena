#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Gui.h"
#include "Input.h"
#include "Renderer.h"
#include "Scene.h"
#include "Shader.h"

namespace
{
    constexpr int INITIAL_WIDTH = 1100;
    constexpr int INITIAL_HEIGHT = 700;
    constexpr float WORLD_HALF_HEIGHT = 100.0f;
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Errore inizializzazione GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        INITIAL_WIDTH,
        INITIAL_HEIGHT,
        "Survivors Arena",
        nullptr,
        nullptr
    );
    if (!window)
    {
        std::cerr << "Errore creazione finestra GLFW\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Registra i callback di input prima di ImGui, cosi' ImGui fa chaining corretto.
    glfwSetFramebufferSizeCallback(window, Input::framebufferSizeCallback);
    glfwSetKeyCallback(window, Input::keyCallback);
    glfwSetCursorPosCallback(window, Input::cursorPositionCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Errore inizializzazione GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // Blocco interno: assicura che Shader e Scene rilascino le risorse OpenGL
    // prima di distruggere il contesto GLFW.
    {
        Shader shader;
        if (!shader.load("shader/vertexshaderC.glsl", "shader/fragmentshaderC.glsl"))
        {
            std::cerr << "Errore caricamento shader\n";
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        Renderer renderer(shader);
        Scene scene;

        Gui gui;
        gui.initialize(window);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            int width = 0;
            int height = 0;
            glfwGetFramebufferSize(window, &width, &height);
            if (width <= 0 || height <= 0)
                continue;

            const float aspect = static_cast<float>(width) / static_cast<float>(height);
            const float halfWidth = WORLD_HALF_HEIGHT * aspect;
            const glm::mat4 projection = glm::ortho(
                -halfWidth, halfWidth,
                -WORLD_HALF_HEIGHT, WORLD_HALF_HEIGHT,
                -1.0f, 1.0f
            );
            renderer.setProjectionMatrix(projection);

            renderer.beginFrame(gui.clearColor());
            renderer.setWireframe(gui.wireframe());
            renderer.drawScene(scene);

            gui.beginFrame();
            gui.draw();
            gui.render();

            glfwSwapBuffers(window);
        }

        scene.clear();
        gui.shutdown();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

