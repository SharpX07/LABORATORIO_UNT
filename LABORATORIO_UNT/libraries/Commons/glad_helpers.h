#pragma once
#include <glad/glad.h>

bool initializeGLAD()
{
	// Initialize GLAD to load OpenGL functions
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction))) {
		return true;
	}

	// Check if the OpenGL version is sufficient
	if (!GLAD_GL_VERSION_3_3) {
		return true;
	}
	return false;
}