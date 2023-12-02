#pragma once

#include <GameRender/model_animation.h>  
#include <GameRender/shader.h>
#include <glm/vec3.hpp>

class Light {
public:
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 color{ 255,255,255 };

	Light() {

	}

	//Return -1 if any attribute didn't load correctly
	int setLightShader(Shader& shader);
};