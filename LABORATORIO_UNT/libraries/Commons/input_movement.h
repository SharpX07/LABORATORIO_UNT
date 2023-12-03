#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <GameRender/camera.h>
#include <SFML/Window/Mouse.hpp>

void keyboard_input(Camera& camera, float deltaTime, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.Zoom = 20;
    }
    else
    {
        camera.Zoom = 80;
    }
}

void mouse_input(Camera& camera, GLFWwindow* window, float centerX, float centerY)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    float xoffset = xpos - centerX;
    float yoffset = -ypos + centerY;

    glfwSetCursorPos(window, centerX, centerY);

    camera.ProcessMouseMovement(xoffset, yoffset);
}
