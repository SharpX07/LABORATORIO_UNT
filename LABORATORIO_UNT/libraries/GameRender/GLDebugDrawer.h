#pragma once

#include <bullet/LinearMath/btIDebugDraw.h>
#include <iostream>
#include <GameRender/camera.h>
#include <GameRender/shader.h>

// Clase que proporciona capacidades de dibujo de depuración para Bullet Physics
class GLDebugDrawer : public btIDebugDraw
{
    int m_debugMode;
    unsigned int VBO, VAO, shaderProgram;

public:
    Camera* camera;        // Puntero a la cámara para obtener la información de vista
    Shader* shaderDebug;   // Puntero al shader utilizado para el dibujo de depuración
    std::vector<float> allVertices;  // Todos los vértices utilizados para el dibujo de líneas

    // Constructor de GLDebugDrawer
    GLDebugDrawer();

    // Método para limpiar y dibujar las líneas acumuladas
    void flushLines();

    // Métodos virtuales heredados de btIDebugDraw

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

    virtual void reportErrorWarning(const char* warningString);

    virtual void draw3dText(const btVector3& location, const char* textString);

    virtual void setDebugMode(int debugMode);

    virtual int getDebugMode() const { return m_debugMode; }
};
