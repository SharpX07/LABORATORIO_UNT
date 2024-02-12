#include <GameRender/GLDebugDrawer.h>

#include <stdio.h> //printf debugging
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Constructor de GLDebugDrawer
GLDebugDrawer::GLDebugDrawer()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 10000000, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// M�todo para dibujar una l�nea
void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    float vertices[] = {
        from.x(), from.y(), from.z(),
        to.x(), to.y(), to.z()
    };

    allVertices.insert(allVertices.end(), std::begin(vertices), std::end(vertices));

    // Verificar si se ha alcanzado el n�mero m�ximo de l�neas antes de vaciar el b�fer
    if (allVertices.size() / 6 >= 10000000)
    {
        flushLines();
    }
}

// M�todo para vaciar y dibujar las l�neas acumuladas
void GLDebugDrawer::flushLines()
{
    shaderDebug->use();
    shaderDebug->setMat4("view", camera->GetViewMatrix());
    shaderDebug->setMat4("projection", camera->getProjectionMatrix());

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Transferir todos los v�rtices a la GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * allVertices.size(), allVertices.data());

    // Dibujar todas las l�neas
    glDrawArrays(GL_LINES, 0, allVertices.size() / 3);

    // Limpiar el b�fer para el pr�ximo conjunto de l�neas
    allVertices.clear();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// M�todo para configurar el modo de depuraci�n
void GLDebugDrawer::setDebugMode(int debugMode)
{
    m_debugMode = debugMode;
}

void GLDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

// M�todo para reportar errores y advertencias (en consola)
void GLDebugDrawer::reportErrorWarning(const char* warningString)
{
    printf(warningString);
}

// M�todo para dibujar puntos de contacto (sin implementaci�n)
void GLDebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}
