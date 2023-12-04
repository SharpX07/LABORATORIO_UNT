#pragma once

#include <GameRender/model_animation.h>  
#include <GameRender/shader.h>
#include <glm/vec3.hpp>

class Light {
public:
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::uvec3 color{ 255,255,255 };

	Light(glm::vec3 position = glm::vec3{ 0,0,0 }, glm::uvec3 color = glm::uvec3{ 0,0,0 });
};