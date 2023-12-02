#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <GameRender/camera.h>
#include <SFML/Window/Mouse.hpp>

void keyboard_input(Camera& camera, float deltaTime)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		camera.Zoom = 20;
	}
	else
		camera.Zoom = 80;
}

void mouse_input(Camera& camera, sf::RenderWindow& window, float centerX, float centerY)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	float xoffset = mousePos.x - centerX;
	float yoffset = -mousePos.y + centerY;
	sf::Mouse::setPosition(sf::Vector2i(centerX, centerY), window);
	camera.ProcessMouseMovement(xoffset, yoffset);
}