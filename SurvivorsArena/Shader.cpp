#include "Shader.h"
#include "ShaderMaker.h"

#include <glm/gtc/type_ptr.hpp>


Shader::~Shader()
{
    destroy();
}


bool Shader::load(
    const char* vertexShaderFile,
    const char* fragmentShaderFile)
{
    destroy();

    m_programId =
        ShaderMaker::createProgram(
            (char*)vertexShaderFile,
            (char*)fragmentShaderFile
        );

    return m_programId != 0;
}


void Shader::setMat4(
    const char* name,
    const glm::mat4& matrix) const
{
    // Recupera la posizione della uniform nel programma.
    if (m_programId == 0) return;

    GLint location = glGetUniformLocation(
        m_programId,
        name
    );

    if (location == -1)
        return;

    // Trasferisce i 16 valori della matrice alla uniform.
    glUniformMatrix4fv(
        location,
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}

void Shader::setFloat(
    const char* name,
    float value) const
{
    if (m_programId == 0) return;

    GLint location = glGetUniformLocation(
        m_programId,
        name
    );

    if (location == -1)
        return;

    glUniform1f(location, value);
}

void Shader::use() const
{
    glUseProgram(m_programId);
}


void Shader::destroy()
{
    if (m_programId != 0)
    {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

