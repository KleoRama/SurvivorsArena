#pragma once

#include <glad/glad.h>
#include "Shader.h"

// sfondo fullscreen in coordinate NDC [-1, +1].
// Resta separato dalla scene 
class BackgroundRenderer
{
public:
    explicit BackgroundRenderer(const Shader& shader);
    ~BackgroundRenderer();

    BackgroundRenderer(const BackgroundRenderer&) = delete;
    BackgroundRenderer& operator=(const BackgroundRenderer&) = delete;

    void draw(float timeSeconds) const;

private:
    const Shader& m_shader;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
};