#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "BackgroundRenderer.h"
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

        // Secondo shader program dedicato solo allo sfondo
        Shader backgroundShader;
        if (!backgroundShader.load("shader/background.vert", "shader/background.frag"))
        {
            std::cerr << "Errore caricamento shader sfondo\n";
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        Renderer renderer(shader);
        BackgroundRenderer background(backgroundShader);
        Scene scene;

        Gui gui;
        gui.initialize(window);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            // Se ho premuto F1, apre o chiude il menu debug
            if (Input::consumeDebugMenuToggle())
                gui.toggleDebugMenu();

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

            // Tempo trascorso dall'avvio, usato dallo shader per animare lo sfondo
            const float now = static_cast<float>(glfwGetTime());

            // Pulisce lo schermo e disegna lo sfondo sempre pieno (mai in wireframe)
            renderer.beginFrame(glm::vec4(0.01f, 0.01f, 0.015f, 1.0f));
            renderer.setWireframe(false);
            background.draw(now);

            // Ripristina il wireframe scelto in ImGui e disegna gli oggetti della scena sopra lo sfondo
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

