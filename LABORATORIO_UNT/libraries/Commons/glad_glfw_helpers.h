#pragma once
#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>

#define ERROR 0

// Funci�n para inicializar GLFW
bool initializeGLFW()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return ERROR;
    }

    // Configuraci�n de GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    return 1;
}

// Funci�n para verificar la creaci�n de la ventana GLFW
bool windowCreated(GLFWwindow* window)
{
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return ERROR;
    }
    return 1;
}

// Funci�n para inicializar GLAD
bool initializeGlad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return ERROR;
    }
    return 1;
}
